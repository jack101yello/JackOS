GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = -32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/gdt.o \
		  obj/drivers/driver.o \
		  obj/hardware/port.o \
		  obj/hardware/interruptstubs.o \
		  obj/hardware/interrupts.o \
		  obj/hardware/pci.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
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
	mkdir isodir
	mkdir isodir/boot
	mkdir isodir/boot/grub
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