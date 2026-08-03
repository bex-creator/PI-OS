# Requires aarch64-linux-gnu-gcc
CC = aarch64-linux-gnu-gcc
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles

all: kernel8.img

boot.o: boot.s
	$(CC) $(CFLAGS) -c boot.s -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel8.elf: boot.o kernel.o
	$(CC) $(CFLAGS) -T linker.ld boot.o kernel.o -o kernel8.elf

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm -f *.o kernel8.elf kernel8.img