GPPPARAMS = -g -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fcheck-new
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
		  obj/gui/widget.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o \
		  obj/filesystem/vfs.o \
		  obj/filesystem/initrd.o \
		  obj/libc/libc.o \
		  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	/home/jack/opt/cross/bin/i386-elf-g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	/home/jack/opt/cross/bin/i386-elf-as $(ASPARAMS) -o $@ $<

jackoskernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

jackos.iso: jackoskernel.bin
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/jackoskernel.bin
	cp initrd.img isodir/boot/initrd.img
	echo 'menuentry "JackOS" {' > isodir/boot/grub/grub.cfg
	echo '	multiboot /boot/jackoskernel.bin' >> isodir/boot/grub/grub.cfg
	echo '	insmod /boot/initrd.img' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o jackos.iso isodir

install: jackos.iso

run: jackos.iso
	cp initrd.img isodir/boot/initrd.img
	qemu-system-i386 -cdrom jackos.iso

.PHONY: clean
clean: 
	rm -rfv isodir obj
	rm -v *.bin *.iso
