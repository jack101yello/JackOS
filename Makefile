CFLAGS?=-O2 -g
CFLAGS:=$(CFLAGS) -Wall -Wextra

SRCS = $(wildcard src/kernel/drivers/*/*.c)
PROGS = $(patsubst src/%.c,build/%.o,$(SRCS))
ASMSRCS = $(wildcard src/kernel/drivers/*/*.asm)
ASMPROGS = $(patsubst src/%.asm,build/%,$(ASMSRCS))
SRCDIR = $(wildcard src/*/)
BUILDDIR = $(patsubst src/%/, build/%/, $(SRCDIR))
DVRSRCDIR = $(wildcard src/kernel/drivers/*/)
DVRBUILDDIR = $(patsubst src/kernel/drivers/%/, build/kernel/drivers/%/, $(DVRSRCDIR))

all: build/jackos.iso

run: build/jackos.iso
	qemu-system-i386 -cdrom build/jackos.iso

build/boot.o: src/boot/boot.s $(BUILDDIR) $(DVRBUILDDIR)
	i686-elf-as $< -o $@

build/kernel.o: src/kernel/kernel.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding

build/jackos.bin: build/boot.o $(ASMPROGS) $(PROGS) build/kernel.o
	i686-elf-gcc -T src/boot/linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

build/%.o: src/%.c
	i686-elf-gcc -c $< -o $@ -std=gnu99 -ffreestanding

build/%: src/%.asm
	nasm -f elf32 $< -o $@

build/jackos.iso: build/jackos.bin
	mkdir -v -p build/isofiles/boot/grub
	cp -v src/boot/grub.cfg build/isofiles/boot/grub/
	cp -v $< build/isofiles/boot/
	grub-mkrescue -o $@ build/isofiles
	rm -v -rf build/isofiles

build/kernel/drivers/%/: src/kernel/drivers/%/
	mkdir -v -p $@

build/%/: src/%/
	mkdir -v -p $@

clean:
	rm -v -r build