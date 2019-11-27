GDB = arm-none-eabi-gdb
OOCD = openocd
OOCDFLAGS = -f interface/stlink.cfg -f target/stm32h7x.cfg

PROJECT = decurrent
BUILDDIR = build

USE_OPT = -O1 -ggdb -falign-functions=16
USE_OPT += -std=gnu11 -ffast-math -Wall -Werror -Wno-unused -Wno-error=maybe-uninitialized

PROJECT_CSRC += src/main.c
PROJECT_CSRC += src/board.c

ULIBS = -lm
ULIBDIR = src
UINCDIR = src

ALLCSRC += $(PROJECT_CSRC)
include Makefile.chibios

program: $(BUILDDIR)/$(PROJECT).elf
	$(OOCD) $(OOCDFLAGS) -c "program $< verify reset exit"

# Chibios's build rules want to generate listings, but they are just waste
# of disk space.
all: rmlists
rmlists:
	rm -rf $(BUILDDIR)/lst/*
