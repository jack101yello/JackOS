GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = -32
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o interruptstubs.o interrupts.o keyboard.o mouse.o kernel.o

%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

jackoskernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: jackoskernel.bin
	mkdir -p isodir/boot/grub
	cp jackoskernel.bin isodir/boot/jackos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o jackos.iso isodir

jackos.iso: install

run: jackos.iso
	qemu-system-i386 -cdrom jackos.iso

clean: 
	rm -v *.o
	rm -v *.bin
	rm -v *.iso
	rm -rfv isodir
