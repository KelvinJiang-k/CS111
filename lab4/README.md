# Hey! I'm Filing Here

In this lab, I successfully implemented the following mountable ext2 file system containing
a symbolic link, text file, root directory, and subdirectory

## Building

make

## Running

./ext2-create
mkdir mnt
sudo mount -o loop cs111-base.img mnt


## Cleaning up

sudo umount mnt
rmdir mnt
make clean