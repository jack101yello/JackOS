# Puts kernel binary into the floppy image file

sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0