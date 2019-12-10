// Helpers for using STM32H7 MDMA

#pragma once
#include <stdint.h>

typedef struct {
    uint32_t CTCR; // Transfer configuration register
    uint32_t CBNDTR; // Block number of data register
    uint32_t CSAR; // Source address register
    uint32_t CDAR; // Destination address register
    uint32_t CBRUR; // Block repeat address update register
    uint32_t CLAR; // Link address register
    uint32_t CTBR; // Trigger and bus selection register
    uint32_t reserved;
    uint32_t CMAR; // Mask address register
    uint32_t CMDR; // Mask data register
} MDMA_Link_Entry_t;

typedef void (*mdma_callback_t)(int channel, void *context);

void mdma_init(void);
void mdma_set_callback(int channel, mdma_callback_t callback, void *context);
