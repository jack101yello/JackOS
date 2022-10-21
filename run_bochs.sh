# Sets up a loopback device, runs Bochs on it, then disconnects

LOOP=$(cat tempstorage) # Find the loop that was chosen earlier

echo "Setting up loop on: $LOOP"
sudo /sbin/losetup $LOOP floppy.img # Set up the loop

# Generate bochsrc.txt
echo "megs: 32" > bochsrc.txt
echo "romimage: file=/usr/share/bochs/BIOS-bochs-latest, address=0xe0000" >> bochsrc.txt
echo "vgaromimage: file=/usr/share/bochs/VGABIOS-elpin-2.40" >> bochsrc.txt
echo "floppya: 1_44=$LOOP, status=inserted" >> bochsrc.txt
echo "boot: a" >> bochsrc.txt
echo "log: bochsout.txt" >> bochsrc.txt
echo "mouse: enabled=0" >> bochsrc.txt
echo "clock: sync=realtime" >> bochsrc.txt
echo "cpu: ips=1000000" >> bochsrc.txt

sudo bochs -f bochsrc.txt # Run bochs
sudo /sbin/losetup -d $LOOP # End the loop

# Remove temporary files
rm tempstorage
rm bochsrc.txt
