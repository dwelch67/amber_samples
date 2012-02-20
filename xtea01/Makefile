
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : xtea01.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

xtea01.o : xtea01.c testdata.h
	$(ARMGNU)-gcc $(COPS) -c xtea01.c -o xtea01.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

xtea01.hex : memmap vectors.o uart.o xtea01.o 
	$(ARMGNU)-ld vectors.o xtea01.o uart.o -T memmap -o xtea01.elf
	$(ARMGNU)-objdump -D xtea01.elf > xtea01.list
	$(ARMGNU)-objcopy xtea01.elf -O ihex xtea01.hex






