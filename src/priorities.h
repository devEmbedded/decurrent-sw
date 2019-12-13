// Thread and interrupt priorities

#pragma once

// Chibios thread priorities are between 2 and 255, with 255 being highest priority.
#define THDPRIO_SHELL           130
#define THDPRIO_USB             200
#define THDPRIO_INPUT_DIGITAL   180
#define THDPRIO_INPUT_USB       170
#define THDPRIO_INPUT_ANALOG    160

// Interrupt priorities are between 15 and 0, with 0 being highest priority
// Interrupts above priority 2 cannot access Chibios functions.
#define IRQPRIO_MDMA              4
#define IRQPRIO_QUADSPI           1
