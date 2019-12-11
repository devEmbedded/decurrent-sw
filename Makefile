GDB = arm-none-eabi-gdb
OOCD = openocd
OOCDFLAGS = -f interface/stlink.cfg -f target/stm32h7x_dual_bank.cfg

PROJECT = decurrent
BUILDDIR = build

# Compilation options
USE_OPT = -O1 -ggdb -g3 -gdwarf-3 -falign-functions=16 --specs=nano.specs -u _printf_float
USE_OPT += -std=gnu11 -ffast-math -Wall -Werror -Wno-unused -Wno-error=maybe-uninitialized

# Files included in project
PROJECT_CSRC += src/decurrent.pb.c
PROJECT_CSRC += src/main.c src/debug.c
PROJECT_CSRC += src/board.c
PROJECT_CSRC += src/usb_thread.c src/usbcfg.c src/usb_hub.c src/shell_cmds.c
PROJECT_CSRC += src/databuf.c src/mdma.c src/output_vsw.c src/input_digital.c src/input_analog.c
PROJECT_CSRC += src/snappy32.c

# External modules
LIBS_CSRC += nanopb/pb_common.c nanopb/pb_encode.c nanopb/pb_decode.c
LIBS_CSRC += CMSIS/CMSIS/DSP/Source/FastMathFunctions/arm_sin_q31.c
LIBS_CSRC += CMSIS/CMSIS/DSP/Source/CommonTables/CommonTables.c

# Search path for includes and libraries
ULIBS = -lm
ULIBDIR = src
UINCDIR = src nanopb CMSIS/CMSIS/DSP/Include

ALLCSRC += $(PROJECT_CSRC) $(LIBS_CSRC)
include Makefile.chibios

src/decurrent.pb.c: src/decurrent.proto src/decurrent.options
	protoc --plugin=nanopb/generator/protoc-gen-nanopb --nanopb_out=. src/decurrent.proto

program: $(BUILDDIR)/$(PROJECT).elf
	$(OOCD) $(OOCDFLAGS) -c "program $< verify reset exit"

debug:
	$(GDB) -iex 'target ext | $(OOCD) -d1 $(OOCDFLAGS) -c "gdb_port pipe"' \
	       -iex 'source gdb_macros' \
		   -iex 'mon halt' $(BUILDDIR)/$(PROJECT).elf

debug_rtos:
	$(GDB) -iex 'target ext | $(OOCD) -d1 $(OOCDFLAGS) -c "stm32h7x.cpu configure -rtos auto;" -c "gdb_port pipe"' \
	       -iex 'source gdb_macros' \
		   -iex 'mon halt' $(BUILDDIR)/$(PROJECT).elf

profile:
	$(OOCD) $(OOCDFLAGS) -c "init" -c "halt" -c "profile 30 gmon.out" -c "resume" -c "exit"
	gprof -b $(BUILDDIR)/$(PROJECT).elf

# Chibios's build rules want to generate listings, but they are just waste
# of disk space.
all: rmlists
rmlists:
	rm -rf $(BUILDDIR)/lst/*
