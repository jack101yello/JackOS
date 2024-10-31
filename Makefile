GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fcheck-new
ASPARAMS = -32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/gdt.o \
		  obj/memorymanagement.o \
		  obj/drivers/driver.o \
		  obj/hardware/port.o \
		  obj/hardware/interruptstubs.o \
		  obj/hardware/interrupts.o \
		  obj/multitasking.o \
		  obj/hardware/pci.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/drivers/vga.o \
		  obj/drivers/ata.o \
		  obj/gui/widget.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o \
		  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

jackoskernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

jackos.iso: jackoskernel.bin
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/jackoskernel.bin
	echo 'menuentry "JackOS" {' > isodir/boot/grub/grub.cfg
	echo '	multiboot /boot/jackoskernel.bin' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o jackos.iso isodir

install: jackos.iso

run: jackos.iso
	qemu-system-i386 -cdrom jackos.iso

.PHONY: clean
clean: 
	rm -rfv isodir obj
	rm -v *.bin *.iso