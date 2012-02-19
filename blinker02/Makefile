
#ARMGNU=arm-thumb-elf
#ARMGNU=arm-none-linux-gnueabi
ARMGNU?=arm-none-eabi

all : blinker02.hex 

blinker02.hex : blinker02.s memmap
	$(ARMGNU)-as  blinker02.s -o blinker02.o
	$(ARMGNU)-ld -o blinker02.elf blinker02.o -T memmap
	$(ARMGNU)-objdump -D blinker02.elf > blinker02.list
	$(ARMGNU)-objcopy blinker02.elf -O ihex blinker02.hex

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.hex


