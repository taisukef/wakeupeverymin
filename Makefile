# gcc Makefile for LPC810
# Kamal Mostafa <kamal@whence.com>
#
# License: Public Domain

CROSS = arm-none-eabi-
#CPU = -mthumb -mcpu=cortex-m0plus -MD
CPU = -mthumb -mcpu=cortex-m0 -MD
FPU = # -mfloat-abi=soft

FILENAME = obj
LINKERSCRIPT = LPC810FN8.ld

OBJDIR = obj

# CFLAGS+= -Os --specs=nano.specs -ffunction-sections -fdata-sections -fno-builtin
CFLAGS+= -Os -ffunction-sections -fdata-sections -fno-builtin

CC = ${CROSS}gcc
LD = ${CROSS}ld
OBJCOPY = ${CROSS}objcopy
SIZE = ${CROSS}size

# use these on the $(LD) line, as needed:
LIBM = ${shell ${CC} ${CFLAGS} --print-file-name=libm.a}
LIBC = ${shell ${CC} ${CFLAGS} --print-file-name=libc.a}
LIBGCC = ${shell ${CC} ${CFLAGS} --print-libgcc-file-name}

CFLAGS += $(CPU) $(FPU) -std=gnu99
LDFLAGS += --gc-sections

INC += -Icmsis
CFLAGS += $(INC) -D__USE_CMSIS # -DCLOCK_SETUP=1

OBJS = gcc_startup_lpc8xx.o system_LPC8xx.o main.o mrt.o

all: $(FILENAME).bin

$(FILENAME).axf: $(LINKERSCRIPT) $(OBJS)
	@$(LD) -o $@ $(LDFLAGS) -T $(LINKERSCRIPT) $(OBJS) $(LIBGCC)
	@$(SIZE) $(FILENAME).axf

clean:
	@rm -f *.o *.d
	@rm -f $(FILENAME).axf $(FILENAME).hex $(FILENAME).bin

%.bin:%.axf
	@$(OBJCOPY) --strip-unneeded -O ihex $(FILENAME).axf $(FILENAME).hex
	@$(OBJCOPY) --strip-unneeded -O binary $(FILENAME).axf $(FILENAME).bin
