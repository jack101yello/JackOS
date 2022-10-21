# Puts kernel binary into the floppy image file

echo "Finding free loop"
sudo losetup -f > tempstorage # Finds a free loop and stores it for later
LOOP=$(cat tempstorage) # Variable to store the loop to use
echo "Loop found: $LOOP"

sudo /sbin/losetup $LOOP floppy.img
sudo mount $LOOP /mnt2
sudo cp -v src/kernel /mnt2/kernel
sudo umount $LOOP
sudo /sbin/losetup -d $LOOP
