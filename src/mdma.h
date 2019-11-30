// Helpers for using STM32H7 MDMA

#pragma once
#include <stdint.h>

typedef struct {
    uint32_t CTCR;
    uint32_t CBNDTR;
    uint32_t CSAR;
    uint32_t CDAR;
    uint32_t CBRUR;
    uint32_t CLAR;
    uint32_t CTBR;
    uint32_t CMAR;
    uint32_t CMDR;
} MDMA_Link_Entry_t;