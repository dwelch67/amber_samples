
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : systick01.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex
	rm -f *.vcd

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

systick01.o : systick01.c
	$(ARMGNU)-gcc $(COPS) -c systick01.c -o systick01.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

systick01.hex : memmap vectors.o uart.o systick01.o 
	$(ARMGNU)-ld vectors.o systick01.o uart.o -T memmap -o systick01.elf
	$(ARMGNU)-objdump -D systick01.elf > systick01.list
	$(ARMGNU)-objcopy systick01.elf -O ihex systick01.hex






