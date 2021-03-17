// 2 channel digital capture using SPI1 and SPI3 peripherals.
// This is used for capturing USB full-speed traffic.

#pragma once

#include <stdint.h>

#define INPUT_SPI_BASEFREQ 96000000

void input_spi_init(void);

uint16_t input_spi_read(void);

void input_spi_start(void);

void input_spi_stop(void);