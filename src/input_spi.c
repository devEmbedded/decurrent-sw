#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "mdma.h"
#include "snappy32.h"
#include "decurrent.pb.h"
#include <pb_encode.h>
#include "priorities.h"
#include "usb_thread.h"
#include "databuf.h"
#include "debug.h"
#include "input_spi.h"

void input_spi_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB1LENR |= RCC_APB1LENR_SPI3EN;
}

uint16_t input_spi_read(void)
{
    SPI1->CFG1 = SPI3->CFG1 = SPI_CFG1_RXDMAEN | (3 << SPI_CFG1_FTHLV_Pos) | SPI_CFG1_DSIZE_Msk;
    SPI1->CFG2 = SPI_CFG2_MASTER | SPI_CFG2_SSOE | SPI_CFG2_SSM | SPI_CFG2_COMM_1;
    SPI3->CFG2 = SPI_CFG2_MASTER | SPI_CFG2_SSOE | SPI_CFG2_SSM | SPI_CFG2_COMM_1 | SPI_CFG2_IOSWP;

    SPI1->CR1 = SPI3->CR1 = SPI_CR1_SPE;

    uint32_t clockconfig = RCC->D2CCIP1R;
    RCC->D2CCIP1R |= RCC_D2CCIP1R_SPI123SEL_Msk;
    SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_CSTART;
    SPI3->CR1 |= SPI_CR1_SSI | SPI_CR1_CSTART;
    RCC->D2CCIP1R = clockconfig;

    while (!(SPI1->SR & SPI_SR_RXWNE));

    uint32_t r1 = SPI1->RXDR;
    uint32_t r2 = SPI3->RXDR;
    //SPI1->CR1 = SPI3->CR1 = 0;

    return ((r1 & 0xFF) << 8) | (r2 & 0xFF);
}

void input_spi_start(void)
{

}

void input_spi_stop(void)
{

}