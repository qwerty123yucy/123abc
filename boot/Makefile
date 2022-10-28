PREFIX=arm-none-eabi
CC=$(PREFIX)-gcc
AS=$(PREFIX)-as
LD=${PREFIX}-ld
CPP=$(PREFIX)-gcc -E


CFLAGS  = -fno-stack-protector -fno-builtin -nostdlib
CFLAGS	+= -I./arch/stm32f103/include/ -I./include/
CFLAGS  += -g -O3 -mcpu=cortex-m3 -std=c17

# libgcc.a is used to calculate float or double values
LDFLAGS = -g -static -L/usr/lib/gcc/arm-none-eabi/12.2.0/thumb/v7-m/nofp/ -lgcc
LD_SCRIPT = ./boot.lds


DEFINES = -DSTACK_SIZE=4096*4 -DMAX_HEAP_SIZE=4096*4


ARCH_ASM_OBJ=boot.o stack.o vectors.o loop.o
ARCH_OBJ=$(addprefix asm/, $(ARCH_ASM_OBJ)) usart.o rcc.o gpio.o exti.o afio.o nvic.o timer.o pwr.o rtc.o adc.o dma.o spi.o spi_flash.o
ARCH_SRC=./arch/stm32f103/

MM_OBJ=mman.o
MM_SRC=./mm/

UTILS_OBJ=shell.o io.o time.o
UTILS_SRC=./utils/

OBJ	:=	$(addprefix $(ARCH_SRC),$(ARCH_OBJ)) $(addprefix $(MM_SRC),$(MM_OBJ)) $(addprefix $(UTILS_SRC),$(UTILS_OBJ))

all:boot

boot:$(OBJ)
	$(LD) -o $@ $(OBJ) $(LDFLAGS) -script $(LD_SCRIPT)
	rm -f $(OBJ)

%.o:%.S
	$(CC) -D__ASSEMBLY__ $(CFLAGS) $(DEFINES) -c -o $@ $<

%.o:%.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<

clean:
	rm -f $(OBJ) boot
