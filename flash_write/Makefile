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
OUT_FILE  = flash_write
MAP_FILE  = $(OUT_FILE).map
EXE_FILE  = $(OUT_FILE).elf
DIS_FILE  = $(OUT_FILE).dis


# Compiler options
MFLAGS    = -march=rv32im -mabi=ilp32
LFLAGS    = -static -L $(DIR_SYSROOT)/lib -Wl,-T,$(DIR_SYSROOT)/include/bmetal/generated/linker_gen.ld \
            -mcmodel=medany $(MFLAGS) -Xlinker -Map=$(MAP_FILE) \
            -Wl,--whole-archive,-lbmetal_crt,--no-whole-archive -lc
INC_PATH  = -I./inc -I./xspi
CFLAGS    = -c -g -O0 $(INC_PATH) $(MFLAGS)
AFLAGS    = -c -g -O0 $(INC_PATH) $(MFLAGS)
DEFINES   =

# Source files
CSRCS     = akaria_menu_table.c akaria_menu.c akaria_stdio.c akaria_PmodSF3.c akaria_sf3_test.c akaria_flash_write.c \
            xspi/xil_assert.c xspi/xspi_options.c xspi/xspi_stats.c xspi/xspi.c \
			main.c
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


