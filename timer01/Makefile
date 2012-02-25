
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : timer01.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

timer01.o : timer01.c
	$(ARMGNU)-gcc $(COPS) -c timer01.c -o timer01.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

timer01.hex : memmap vectors.o uart.o timer01.o 
	$(ARMGNU)-ld vectors.o timer01.o uart.o -T memmap -o timer01.elf
	$(ARMGNU)-objdump -D timer01.elf > timer01.list
	$(ARMGNU)-objcopy timer01.elf -O ihex timer01.hex






