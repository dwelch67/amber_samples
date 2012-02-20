
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : xxtea01.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

xxtea01.o : xxtea01.c testdata.h
	$(ARMGNU)-gcc $(COPS) -c xxtea01.c -o xxtea01.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

xxtea01.hex : memmap vectors.o uart.o xxtea01.o 
	$(ARMGNU)-ld vectors.o xxtea01.o uart.o -T memmap -o xxtea01.elf
	$(ARMGNU)-objdump -D xxtea01.elf > xxtea01.list
	$(ARMGNU)-objcopy xxtea01.elf -O ihex xxtea01.hex






