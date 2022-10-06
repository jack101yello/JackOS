# This is the Makefile for JackOS

# Note that C and C++ rules are set up by default

SOURCES=boot.o main.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
LDFLAGS=-Tlink.ld -m elf_i386
ASFLAGS=-felf

all: $(SOURCES) link # Compiles everything

clean: # Cleans everything for a fresh compilation
	-rm *.o kernel

link: # Links all of the compiled files
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o: # Compiled the object files with nasm
	nasm $(ASFLAGS) $<