
#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a

all : hello_world.hex

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.elf
	rm -f *.list
	rm -f *.hex

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

hello_world.o : hello_world.c
	$(ARMGNU)-gcc $(COPS) -c hello_world.c -o hello_world.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o

hello_world.hex : memmap vectors.o uart.o hello_world.o 
	$(ARMGNU)-ld vectors.o hello_world.o uart.o -T memmap -o hello_world.elf
	$(ARMGNU)-objdump -D hello_world.elf > hello_world.list
	$(ARMGNU)-objcopy hello_world.elf -O ihex hello_world.hex






