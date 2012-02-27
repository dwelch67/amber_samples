

#ARMGNU ?= arm-none-linux-gnueabi
ARMGNU = arm-none-eabi


COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=arm2 -mtune=arm2 -march=armv2a -mno-thumb-interwork
AOPS = --warn  -mcpu=arm2 -march=armv2a


ZHEADERS = zlib.h zconf.h deflate.h zutil.h crc32.h crc32.c trees.h gzguts.h inflate.h inftrees.h inffast.h inffixed.h

ZOBJS = compress.o deflate.o zutil.o adler32.o crc32.o trees.o gzwrite.o uncompr.o inflate.o inftrees.o inffast.o

all : twain.hex

clean :
	rm -f *.o
	rm -f *.list
	rm -f *.elf
	rm -f *.hex
	rm -f *.vcd


vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

twain.o : twain.c
	$(ARMGNU)-gcc $(COPS) -c twain.c -o twain.o

uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -c uart.c -o uart.o


compress.o : compress.c
	$(ARMGNU)-gcc $(COPS) -c compress.c -o compress.o

deflate.o : deflate.c
	$(ARMGNU)-gcc $(COPS) -c deflate.c -o deflate.o

zutil.o : zutil.c
	$(ARMGNU)-gcc $(COPS) -c zutil.c -o zutil.o

adler32.o : adler32.c
	$(ARMGNU)-gcc $(COPS) -c adler32.c -o adler32.o

crc32.o : crc32.c
	$(ARMGNU)-gcc $(COPS) -c crc32.c -o crc32.o

trees.o : trees.c
	$(ARMGNU)-gcc $(COPS) -c trees.c -o trees.o

gzwrite.o : gzwrite.c
	$(ARMGNU)-gcc $(COPS) -c gzwrite.c -o gzwrite.o

gzlib.o : gzlib.c
	$(ARMGNU)-gcc $(COPS) -c gzlib.c -o gzlib.o

uncompr.o : uncompr.c
	$(ARMGNU)-gcc $(COPS) -c uncompr.c -o uncompr.o

inflate.o : inflate.c
	$(ARMGNU)-gcc $(COPS) -c inflate.c -o inflate.o

inftrees.o : inftrees.c
	$(ARMGNU)-gcc $(COPS) -c inftrees.c -o inftrees.o

inffast.o : inffast.c
	$(ARMGNU)-gcc $(COPS) -c inffast.c -o inffast.o

GCCLIB = gcclib/_udivsi3.o gcclib/_idivsi3.o  gcclib/_divsi3.o

twain.hex : memmap vectors.o uart.o twain.o $(ZOBJS) $(GCCLIB)
	$(ARMGNU)-ld -T memmap vectors.o twain.o uart.o -o twain.elf $(ZOBJS) $(GCCLIB)
	$(ARMGNU)-objdump -D twain.elf > twain.list
	$(ARMGNU)-objcopy twain.elf -O ihex twain.hex 




