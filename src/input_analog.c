#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "databuf.h"
#include "decurrent.pb.h"
#include <pb_encode.h>
#include "priorities.h"
#include "cic_filter.h"
#include "input_analog.h"

typedef struct {
    const stm32_dma_stream_t *dma_stream;
    ADC_TypeDef *adc;
    msg_t queue_mem[DATABUF_COUNT];
    mailbox_t queue;
    const char *thread_name;
    thread_t *thread;
    int divider;
    downsample32_state_t ds32_state;
    downsampleX_state_t dsX_state;
} adc_state_t;

static adc_state_t g_adc1_state;
static adc_state_t g_adc2_state;
static adc_state_t g_adc3_state;

static void input_analog_dma_irq(void *p, uint32_t flags)
{
    adc_state_t *state = p;
    if (flags & STM32_DMA_ISR_TCIF)
    {
        if (state->dma_stream->stream->CR & DMA_SxCR_CT)
        {
            // Buffer 0 filled, transfer ongoing to buffer 1
            chMBPostI(&state->queue, (msg_t)state->dma_stream->stream->M0AR);
            state->dma_stream->stream->M0AR = (uint32_t)databuf_try_allocate(DOMAIN_D2);
        }
        else
        {
            // Buffer 1 filled, transfer ongoing to buffer 0
            chMBPostI(&state->queue, (msg_t)state->dma_stream->stream->M1AR);
            state->dma_stream->stream->M1AR = (uint32_t)databuf_try_allocate(DOMAIN_D2);
        }
    }
}


// Thread to compress filled buffers and stuff them into protobuf messages.
static thread_t *g_input_analog_thread;
static THD_WORKING_AREA(g_input_analog_thread_wa, 4096);
static THD_FUNCTION(input_analog_thread, arg) {
    adc_state_t *state = arg;
    chRegSetThreadName(state->thread_name);

    int64_t sampleidx = 0;
    databuf_t *input_buf = 0;
    size_t input_buf_index = 0;

    while (!chThdShouldTerminateX())
    {
        if (!input_buf)
        {
            if (chMBFetchTimeout(&state->queue, (msg_t*)&input_buf, TIME_MS2I(100)) != MSG_OK)
                continue;
            input_buf_index = 0;
            cacheBufferInvalidate(input_buf, DATABUF_BYTES);
        }

        databuf_t *output_buf = databuf_allocate(DOMAIN_D1);
        pb_ostream_t stream = pb_ostream_from_buffer((uint8_t*)output_buf, DATABUF_BYTES);
        
        // Leave 4 bytes for the packet length prefix
        pb_ostream_t prefix_stream = stream;
        uint32_t dummy = 0;
        pb_encode_fixed32(&stream, &dummy);

        {
            // Write packet header
            USBResponse packet = USBResponse_init_zero;
            packet.status = Status_STATUS_MEASURING;
            if (state == &g_adc1_state) packet.channelidx = ChannelId_CHANNEL_ANALOG_V1;
            if (state == &g_adc2_state) packet.channelidx = ChannelId_CHANNEL_ANALOG_V2;
            if (state == &g_adc3_state) packet.channelidx = ChannelId_CHANNEL_ANALOG_I1;
            packet.samplerate = INPUT_ANALOG_BASEFREQ / state->divider;
            packet.sampleidx = sampleidx;
            packet.bits_per_sample = (state->divider == 1) ? 16 : 32;
            packet.cpu_usage = get_cpu_usage();
            packet.compression = Compression_COMPRESSION_DELTA;

            if (!pb_encode(&stream, USBResponse_fields, &packet))
            {
                dbg("pb_encode failed: %s", stream.errmsg);
            }
        }

        // Reserve 3 bytes for the field tag + data length
        pb_ostream_t tag_stream = stream;
        pb_write(&stream, (const uint8_t*)"\0\0\0", 3);
        
        if (state->divider == 1)
        {
            // Write 16-bit data / deltas directly
        }
        else
        {
            // Downsample
            size_t count = DATABUF_BYTES / sizeof(uint16_t);
            count = downsample32(&state->ds32_state, (uint16_t*)input_buf, count, input_buf->data);

            if (state->divider > 3)
            {
                count = downsampleX(&state->dsX_state, input_buf->data, count, input_buf->data)
            }

            // Write deltas
            int32_t prev_value = 0;
            for (int i = 0; i < count; i++)
            {
                int32_t sample = (int32_t)input_buf->data[i];
                int32_t delta = sample - prev_value;
                prev_value = sample;

                
            }
        }


    }
}

static void init_channel(adc_state_t *state, const char *thread_name, ADC_TypeDef *adc, uint32_t dma_stream, uint32_t dma_req)
{
    chMBObjectInit(&state->queue, state->queue_mem, DATABUF_COUNT);
    state->adc = adc;
    state->thread_name = thread_name;
    state->divider = 1;
    state->dma_stream = dmaStreamAlloc(dma_stream, IRQPRIO_ADCDMA, input_analog_dma_irq, state);
    dmaSetRequestSource(state->dma_stream, dma_req);
}

void input_analog_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
    RCC->AHB4ENR |= RCC_AHB4ENR_ADC3EN;
    RCC->APB4ENR |= RCC_APB4ENR_VREFEN;
    VREFBUF->CSR = VREFBUF_CSR_ENVR;

    init_channel(&g_adc1_state, "ADC1", ADC1, STM32_DMA1_STREAM0, STM32_DMAMUX1_ADC1);
    init_channel(&g_adc2_state, "ADC2", ADC2, STM32_DMA2_STREAM0, STM32_DMAMUX1_ADC2);
    init_channel(&g_adc3_state, "ADC3", ADC3, STM32_DMA1_STREAM1, STM32_DMAMUX1_ADC3);
}
