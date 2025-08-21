binpath = /home/jack/opt/cross/bin
sysroot = /home/jack/JackOS/sysroot

GPPPARAMS = -g -Iinclude -Ilibc/include -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fcheck-new
ASPARAMS = -32
LDPARAMS = -melf_i386 -z noexecstack

objects = obj/loader.o \
		  obj/gdt.o \
		  obj/memorymanagement.o \
		  obj/drivers/driver.o \
		  obj/hardware/port.o \
		  obj/hardware/interruptstubs.o \
		  obj/hardware/interrupts.o \
		  obj/syscalls.o \
		  obj/multitasking.o \
		  obj/hardware/pci.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/drivers/vga.o \
		  obj/drivers/pit.o \
		  obj/gui/widget.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o \
		  obj/filesystem/vfs.o \
		  obj/filesystem/initrd.o \
		  obj/filesystem/ELF/elfloader.o \
		  obj/libc/libc.o \
		  obj/common/common.o \
		  obj/kernel.o

libk_objs = \
			libc/stdio/print.o \
			libc/stdio/putchar.o \
			libc/stdio/puts.o \
			libc/stdlib/abort.o \
			libc/string/memcmp.o \
			libc/string/memcpy.o \
			libc/string/memmove.o \
			libc/string/memset.o \
			libc/string/strlen.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(binpath)/i386-elf-g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	$(binpath)/i386-elf-as $(ASPARAMS) -o $@ $<

libc/%.o: libc/%.c
	$(binpath)/i386-elf-gcc --sysroot=$(sysroot) -Ilibc/include -ffreestanding -nostdlib -O2 -o $@ $<

jackoskernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

initrd.elf:
	gcc Initrd/makeinitrd.c -o makeinitrd.out
	cp Initrd/ProgramFiles/testfile.txt testfile.txt
	./makeinitrd.out testfile.txt testfile.txt
	rm testfile.txt

jackos.iso: jackoskernel.bin initrd.elf
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/jackoskernel.bin
	cp initrd.elf isodir/boot/initrd.elf
	cp ~/JackOSPrograms/hello.elf isodir/boot/hello.elf
	echo 'menuentry "JackOS" {' > isodir/boot/grub/grub.cfg
	echo '	multiboot /boot/jackoskernel.bin' >> isodir/boot/grub/grub.cfg
	echo '	module /boot/initrd.elf initrd.elf' >> isodir/boot/grub/grub.cfg
	echo '	module /boot/hello.elf hello.elf' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o jackos.iso isodir

install: jackos.iso

libk-includes:
	mkdir -p sysroot/usr/include sysroot/usr/lib
	cp -R libc/include/. sysroot/usr/include

libk-objects: $(libk_objs)
	echo "/// Make objects"

install-libc: libk-includes libk-objects
	# $(binpath)/i386-elf-ar rcs libc/libk.a 

run: jackos.iso
	qemu-system-i386 -cdrom jackos.iso

debug: jackos.iso
	qemu-system-i386 -s -S -cdrom jackos.iso

.PHONY: clean install install-libc run
clean: 
	rm -rfv isodir obj sysroot
	rm -v *.bin *.iso *.elf *.out
	rm -v $(libk_objs)
