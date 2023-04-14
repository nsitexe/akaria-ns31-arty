# SDK-P Setting
DIR_SYSROOT=../../sysroot

# Toolchain
# RV_PREFIX = riscv32-unknown-elf-
# RV_PREFIX = /home/katano/tools/riscv_entry_kit/bin/riscv64-unknown-elf-
# RV_PREFIX = /opt/riscv/bin/riscv64-unknown-elf-
RV_PREFIX = riscv64-unknown-elf-
CC        = $(RV_PREFIX)gcc
CPP       = $(RV_PREFIX)cpp

# Output files
OUT_FILE  = akaria_ns_bsp
MAP_FILE  = $(OUT_FILE).map
EXE_FILE  = $(OUT_FILE).elf
DIS_FILE  = $(OUT_FILE).dis
BIN_FILE  = $(OUT_FILE).bin
BIN_TEXT  = $(OUT_FILE)_bin.h


# Compiler options
MFLAGS    = -march=rv32im -mabi=ilp32
LFLAGS    = -static -L $(DIR_SYSROOT)/lib -Wl,-T,$(DIR_SYSROOT)/include/bmetal/generated/linker_gen.ld \
            -mcmodel=medany $(MFLAGS) -Xlinker -Map=$(MAP_FILE) \
            -Wl,--whole-archive,-lbmetal_crt,--no-whole-archive -lc
INC_PATH  = -I../include 
CFLAGS    = -c -g -O2 $(INC_PATH) $(MFLAGS)
AFLAGS    = -c -g -O2 $(INC_PATH) $(MFLAGS)
DEFINES   =

# Source files
CSRCS     = akaria_led.c  akaria_led_test.c  akaria_timer.c akaria_menu_table.c akaria_menu.c akaria_stdio.c main.c
ASRCS     = 

# Create objects 
COBJS     = $(CSRCS:%.c=%.o)
AOBJS     = $(ASRCS:%.S=%.o)
OBJS      = $(COBJS)  $(AOBJS)

# Dependencies
all: $(EXE_FILE) $(DIS_FILE)
$(EXE_FILE): $(OBJS) $(LK_FILE)

# Assembler
.S.o:
	$(CC) $(AFLAGS) $(DEFINES) -o $@ $<

# Compiler
.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $<

# Linker
$(EXE_FILE):
	$(CC) $(LFLAGS) $(DEFINES) -o $(EXE_FILE) $(OBJS) $(TXLIB)

# For debugging purposes and objdump of our output file.
$(DIS_FILE): $(EXE_FILE)
	$(RV_PREFIX)objdump -D $(EXE_FILE) > $(DIS_FILE)


# Binary File to write SPI Flash
$(BIN_FILE): $(EXE_FILE)
	$(RV_PREFIX)objcopy -O binary --only-section=.reset --only-section=.init --only-section=.text \
    --only-section=.fini --only-section=.comm --only-section=.initdata \
    --only-section=.auxdata  --only-section=.rodata --only-section=.preinit_array \
    --only-section=.init_array --only-section=.fini_array --only-section=.data \
    --only-section=.sdata $(EXE_FILE) $(BIN_FILE)

# Header file to write SPI Flash
.PHONY: bin_header
bin_header: $(BIN_FILE)
	xxd -i -c 16 $(BIN_FILE) > $(BIN_TEXT)
	sed -i "s/unsigned/const unsigned/" $(BIN_TEXT)

# Remove output files and .o files
clean:
	rm $(EXE_FILE) $(DIS_FILE) $(OBJS) $(MAP_FILE) $(BIN_FILE) $(BIN_TEXT)


