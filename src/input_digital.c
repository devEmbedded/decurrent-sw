#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "mdma.h"
#include "deflate.h"
#include "decurrent.pb.h"
#include <pb_encode.h>
#include "usb_thread.h"
#include "databuf.h"
#include "debug.h"
#include "input_digital.h"

#define MDMA_CHANNEL MDMA_Channel0

event_source_t g_input_digital_trigger_event;
static uint32_t g_digital_input_mask;
static uint32_t g_digital_input_divider;
static databuf_t *g_digital_input_current_buffer;
static databuf_t *g_digital_input_next_buffer;

static msg_t g_digital_input_queue_mem[DATABUF_COUNT];
static mailbox_t g_digital_input_queue;

// MDMA linked list entries must be in AXI SRAM, i.e. not in TCM
__attribute__((section(".ram0"), aligned(32)))
static MDMA_Link_Entry_t g_digital_input_mdma_entry;

void input_digital_init(void)
{
    chEvtObjectInit(&g_input_digital_trigger_event);
    chMBObjectInit(&g_digital_input_queue, g_digital_input_queue_mem, DATABUF_COUNT);
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN | RCC_AHB3ENR_MDMAEN;
}

// Swaps 0x76543210 to 0x46570213 to account for QuadSPI data order.
static uint32_t quadspi_preprocess(uint32_t x)
{
    uint32_t swap = ((x ^ (x >> 12)) & 0x000F000F) * 0x1001;
    return x ^ swap;
}

static uint32_t quadspi_preprocess_mask(uint32_t x)
{
    uint32_t swap = ((x ^ (x >> 12)) & 0x000F000F) * 0x1001;
    return (x ^ swap) & g_digital_input_mask;
}

// Set up the next MDMA buffer to be filled
static void input_digital_mdma_callback(int channel, void *context)
{
    uint32_t flags = MDMA_CHANNEL->CISR;
    if (flags & MDMA_CISR_BTIF)
    {
        // Buffer transfer complete, set up next buffer
        MDMA_CHANNEL->CIFCR = MDMA_CIFCR_CBTIF;
        
        chSysLockFromISR();
        chMBPostI(&g_digital_input_queue, (msg_t)g_digital_input_current_buffer);
        chSysUnlockFromISR();

        g_digital_input_current_buffer = g_digital_input_next_buffer;
        g_digital_input_next_buffer = databuf_try_allocate();
        
        if (g_digital_input_next_buffer)
        {
            g_digital_input_mdma_entry.CDAR = (uint32_t)g_digital_input_next_buffer;
        }
        else
        {
            dbg("Digital input failed allocating next buffer");
            g_digital_input_mdma_entry.CDAR = 0;
            g_digital_input_mdma_entry.CBNDTR = 0;
            g_digital_input_mdma_entry.CLAR = 0;
        }
    }
    
    if (flags & MDMA_CISR_TEIF)
    {
        MDMA_CHANNEL->CIFCR = MDMA_CIFCR_CTEIF;
        dbg("Digital input MDMA error: CESR = 0x%08x", MDMA_CHANNEL->CESR);
    }
}

// Thread to compress filled buffers and stuff them into protobuf messages.
static thread_t *g_input_digital_thread;
static THD_WORKING_AREA(g_input_digital_thread_wa, 16384);
static THD_FUNCTION(input_digital_thread, arg) {
    chRegSetThreadName("DigIn");

    int64_t sampleidx = 0;
    databuf_t *input_buf = 0;
    size_t input_buf_index = 0;

    while (!chThdShouldTerminateX())
    {
        if (!input_buf)
        {
            if (chMBFetchTimeout(&g_digital_input_queue, (msg_t*)&input_buf, TIME_MS2I(100)) != MSG_OK)
                continue;
            input_buf_index = 0;
        }

        if (input_buf)
        {
            cacheBufferInvalidate(input_buf, DATABUF_BYTES);
        }
        
        databuf_t *output_buf = databuf_allocate();
        pb_ostream_t stream = pb_ostream_from_buffer((uint8_t*)output_buf, DATABUF_BYTES);
        
        // Leave 4 bytes for the packet length prefix
        pb_ostream_t prefix_stream = stream;
        uint32_t dummy = 0;
        pb_encode_fixed32(&stream, &dummy);

        {
            // Write packet header
            USBResponse packet = USBResponse_init_zero;
            packet.status = Status_STATUS_MEASURING;
            packet.channelidx = ChannelId_CHANNEL_DIGITAL;
            packet.samplerate = INPUT_DIGITAL_BASEFREQ / g_digital_input_divider;
            packet.sampleidx = sampleidx;
            packet.bits_per_sample = 8;
            packet.cpu_usage = get_cpu_usage();
            packet.compression = Compression_COMPRESSION_DEFLATE;

            if (!pb_encode(&stream, USBResponse_fields, &packet))
            {
                dbg("pb_encode failed: %s", stream.errmsg);
            }
        }

        // Leave 3 bytes for the field tag + data length, and align to a word boundary
        size_t data_start_offset = stream.bytes_written + 3;
        if (data_start_offset & 3)
        {
            data_start_offset += 4 - (data_start_offset & 3);
        }
        
        {
            uint32_t starttime = chSysGetRealtimeCounterX();
            int64_t startidx = sampleidx;

            // Initialize deflate compression table
            deflate_preprocessor_t preprocess = (g_digital_input_mask == 0xFFFFFFFF) ? quadspi_preprocess : quadspi_preprocess_mask;
            deflate_state_t deflate_state;
            deflate_init(&deflate_state, preprocess, input_buf->data, DATABUF_WORDS);

            // Compress buffers until input queue empty or output block full
            deflate_start_block(&deflate_state, output_buf->data, DATABUF_WORDS - (data_start_offset / 4 + 2));

            while (input_buf && deflate_state.stream.wr_pos < deflate_state.stream.end_pos)
            {
                size_t len = deflate_compress(&deflate_state, &input_buf->data[input_buf_index], DATABUF_WORDS - input_buf_index);
                sampleidx += len * 4;
                input_buf_index += len;

                if (input_buf_index >= DATABUF_WORDS)
                {
                    // Try to fetch next input block
                    databuf_release(&input_buf);
                    input_buf_index = 0;
                    chMBFetchTimeout(&g_digital_input_queue, (msg_t*)&input_buf, TIME_IMMEDIATE);

                    if (input_buf)
                    {
                        cacheBufferInvalidate(input_buf, DATABUF_BYTES);
                    }
                }
            }

            deflate_end_block(&deflate_state);

            // Write field tag and data length
            size_t words_written = deflate_state.stream.wr_pos - output_buf->data;
            pb_encode_tag(&stream, PB_WT_STRING, USBResponse_data_tag);
            pb_encode_varint(&stream, words_written * 4);
            while (stream.bytes_written < data_start_offset)
            {
                // Pad the length tag (varints can be extended with zero-valued bits)
                *((pb_byte_t*)stream.state - 1) |= 0x80;
                uint8_t buf = 0x00;
                pb_write(&stream, &buf, 1);
            }
            
            uint32_t total_len = stream.bytes_written + words_written * 4 - 4;
            pb_encode_fixed32(&prefix_stream, &total_len);
            usb_thread_write(output_buf);
            output_buf = NULL;

            dbg("%d samples compressed in %d cycles", (int)(sampleidx - startidx), (int)(chSysGetRealtimeCounterX() - starttime));
        }
    }

    databuf_release(&input_buf);
}

// Read a single value from the input pins
uint8_t input_digital_read(void)
{
    QUADSPI->DCR = QUADSPI_DCR_FSIZE_Msk;
    QUADSPI->DLR = 3;
    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    QUADSPI->CR = QUADSPI_CR_DFM | (15 << QUADSPI_CR_FTHRES_Pos) | QUADSPI_CR_EN;
    assert(!(QUADSPI->SR & QUADSPI_SR_BUSY));
    QUADSPI->CCR = (1 << QUADSPI_CCR_FMODE_Pos) | (3 << QUADSPI_CCR_DMODE_Pos) | (2 << QUADSPI_CCR_DCYC_Pos);
    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    uint32_t result = quadspi_preprocess(QUADSPI->DR);
    QUADSPI->CR = 0;
    return result;
}

void input_digital_config(int samplerate, uint8_t channel_mask)
{
    int divider = INPUT_DIGITAL_BASEFREQ / samplerate;
    if (divider < 1) divider = 1;
    if (divider > 1) divider &= ~1;
    g_digital_input_divider = divider;
    g_digital_input_mask = channel_mask * 0x01010101UL;
}

void input_digital_start(void)
{
    chMBReset(&g_digital_input_queue);
    chMBResumeX(&g_digital_input_queue);

    g_input_digital_thread = chThdCreateStatic(g_input_digital_thread_wa, sizeof(g_input_digital_thread_wa), NORMALPRIO, input_digital_thread, NULL);

    g_digital_input_current_buffer = databuf_try_allocate();
    g_digital_input_next_buffer = databuf_try_allocate();
    assert(g_digital_input_current_buffer && g_digital_input_next_buffer);

    mdma_set_callback(0, input_digital_mdma_callback, NULL);

    g_digital_input_mdma_entry.CTCR = MDMA_CTCR_BWM | MDMA_CTCR_PKE | (15 << MDMA_CTCR_TLEN_Pos) | (0 << MDMA_CTCR_DBURST_Pos) | \
                                      (1 << MDMA_CTCR_SBURST_Pos) | (3 << MDMA_CTCR_DINCOS_Pos) | (3 << MDMA_CTCR_DSIZE_Pos) | \
                                      (2 << MDMA_CTCR_SSIZE_Pos) | (2 << MDMA_CTCR_DINC_Pos);
    g_digital_input_mdma_entry.CBNDTR = DATABUF_BYTES;
    g_digital_input_mdma_entry.CSAR = (uint32_t)&QUADSPI->DR;
    g_digital_input_mdma_entry.CDAR = (uint32_t)g_digital_input_next_buffer;
    g_digital_input_mdma_entry.CLAR = (uint32_t)&g_digital_input_mdma_entry;
    g_digital_input_mdma_entry.CTBR = 22; // quadspi_ft_trg
    g_digital_input_mdma_entry.CMAR = 0;
    g_digital_input_mdma_entry.CMDR = 0;
    cacheBufferFlush(&g_digital_input_mdma_entry, sizeof(g_digital_input_mdma_entry));

    MDMA_CHANNEL->CIFCR = ~0; // Clear all interrupt flags
    MDMA_CHANNEL->CTCR   = g_digital_input_mdma_entry.CTCR;
    MDMA_CHANNEL->CBNDTR = g_digital_input_mdma_entry.CBNDTR;
    MDMA_CHANNEL->CSAR   = g_digital_input_mdma_entry.CSAR;
    MDMA_CHANNEL->CDAR   = (uint32_t)g_digital_input_current_buffer;
    MDMA_CHANNEL->CLAR   = g_digital_input_mdma_entry.CLAR;
    MDMA_CHANNEL->CTBR   = g_digital_input_mdma_entry.CTBR;
    MDMA_CHANNEL->CMAR   = g_digital_input_mdma_entry.CMAR;
    MDMA_CHANNEL->CMDR   = g_digital_input_mdma_entry.CMDR;
    MDMA_CHANNEL->CCR    = (2 << MDMA_CCR_PL_Pos) | MDMA_CCR_BTIE | MDMA_CCR_TEIE | MDMA_CCR_EN;

    QUADSPI->DCR = QUADSPI_DCR_FSIZE_Msk;
    QUADSPI->DLR = 0xFFFFFFFF;
    QUADSPI->CR = ((g_digital_input_divider / 2 - 1) << QUADSPI_CR_PRESCALER_Pos) | QUADSPI_CR_DFM | (15 << QUADSPI_CR_FTHRES_Pos) | QUADSPI_CR_EN;

    // Writing CCR will start the transfer
    uint32_t ddr_mode = (g_digital_input_divider == 1) ? QUADSPI_CCR_DDRM : 0;
    QUADSPI->CCR = ddr_mode | (1 << QUADSPI_CCR_FMODE_Pos) | (3 << QUADSPI_CCR_DMODE_Pos) | (2 << QUADSPI_CCR_DCYC_Pos);
}