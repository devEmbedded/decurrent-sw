GDB = arm-none-eabi-gdb
OOCD = openocd
OOCDFLAGS = -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg

PROJECT = decurrent
BUILDDIR = build

# Compilation options
USE_OPT = -O1 -ggdb -falign-functions=16 --specs=nano.specs -u _printf_float
USE_OPT += -std=gnu11 -ffast-math -Wall -Werror -Wno-unused -Wno-error=maybe-uninitialized

# Files included in project
PROJECT_CSRC += src/main.c src/debug.c
PROJECT_CSRC += src/board.c
PROJECT_CSRC += src/usb_thread.c src/usbcfg.c src/shell_cmds.c

# Search path for includes and libraries
ULIBS = -lm
ULIBDIR = src
UINCDIR = src

ALLCSRC += $(PROJECT_CSRC)
include Makefile.chibios

program: $(BUILDDIR)/$(PROJECT).elf
	$(OOCD) $(OOCDFLAGS) -c "program $< verify reset exit"

debug:
	$(GDB) -iex 'target ext | $(OOCD) -d1 $(OOCDFLAGS) -c "gdb_port pipe"' \
	       -iex 'source gdb_macros' \
		   -iex 'mon halt' $(BUILDDIR)/$(PROJECT).elf

# Chibios's build rules want to generate listings, but they are just waste
# of disk space.
all: rmlists
rmlists:
	rm -rf $(BUILDDIR)/lst/*
