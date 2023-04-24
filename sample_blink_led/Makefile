# akaria-bmetal Setting
DIR_SYSROOT=../../sysroot

# Toolchain
RV_PREFIX = riscv64-unknown-elf-
CC        = $(RV_PREFIX)gcc
CPP       = $(RV_PREFIX)cpp

# Output files
OUT_FILE  = sample_blink_led
MAP_FILE  = $(OUT_FILE).map
EXE_FILE  = $(OUT_FILE).elf
DIS_FILE  = $(OUT_FILE).dis

# Compiler options
MFLAGS    = -march=rv32imafc -mabi=ilp32f
LFLAGS    = -static -L $(DIR_SYSROOT)/lib -Wl,-T,$(DIR_SYSROOT)/include/bmetal/generated/linker_gen.ld \
            -mcmodel=medany $(MFLAGS) -Xlinker -Map=$(MAP_FILE) \
            -Wl,--whole-archive,-lbmetal_crt,--no-whole-archive -lc
INC_PATH  = -I. -I../include
CFLAGS    = -c -g -O2 $(INC_PATH) $(MFLAGS)
AFLAGS    = -c -g -O2 $(INC_PATH) $(MFLAGS)
DEFINES   =

# Source files
CSRCS     = akaria_timer.c akaria_led.c main.c
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

# Remove output files and .o files
clean:
	rm $(EXE_FILE) $(DIS_FILE) $(OBJS) $(MAP_FILE)

