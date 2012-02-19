
#ARMGNU=arm-thumb-elf
#ARMGNU=arm-none-linux-gnueabi
ARMGNU?=arm-none-eabi

all : blinker01.hex dumphex

dumphex : dumphex.c
	gcc -o dumphex dumphex.c

blinker01.hex : blinker01.s memmap
	$(ARMGNU)-as  blinker01.s -o blinker01.o
	$(ARMGNU)-ld -o blinker01.elf blinker01.o -T memmap
	$(ARMGNU)-objdump -D blinker01.elf > blinker01.list
	$(ARMGNU)-objcopy blinker01.elf -O ihex blinker01.hex

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.hex
	rm -f dumphex


