
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : tea01.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

tea01.o : tea01.c testdata.h
	$(ARMGNU)-gcc $(COPS) -c tea01.c -o tea01.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

tea01.hex : memmap vectors.o uart.o tea01.o 
	$(ARMGNU)-ld vectors.o tea01.o uart.o -T memmap -o tea01.elf
	$(ARMGNU)-objdump -D tea01.elf > tea01.list
	$(ARMGNU)-objcopy tea01.elf -O ihex tea01.hex






