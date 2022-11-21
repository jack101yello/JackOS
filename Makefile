CFLAGS?=-O2 -g
CFLAGS:=$(CFLAGS) -Wall -Wextra

SRCS = $(wildcard kernel/drivers/*/*.c)
PROGS = $(patsubst %.c,%.o,$(SRCS))
ASMSRCS = $(wildcard kernel/drivers/*/*.asm)
ASMPROGS = $(patsubst %.asm,%,$(ASMSRCS))

all: jackos.iso

run: jackos.iso
	qemu-system-i386 -cdrom jackos.iso

boot.o: boot/boot.s
	i686-elf-as $< -o $@

kernel.o: kernel/kernel.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding

jackos.bin: boot.o $(ASMPROGS) $(PROGS) kernel.o
	i686-elf-gcc -T boot/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

%.o: %.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding

%: %.asm
	nasm -f elf32 $< -o $@

jackos.iso: jackos.bin
	mkdir -v -p isofiles/boot/grub
	cp -v boot/grub.cfg isofiles/boot/grub/
	cp -v $< isofiles/boot/
	grub-mkrescue -o $@ isofiles
	rm -v -rf isofiles

clean:
	rm -v -f *.o
	rm -v -f jackos.bin
	rm -v -f jackos.iso
	rm -v -f $(PROGS)
	rm -v -f $(ASMPROGS)
