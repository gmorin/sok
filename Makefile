#########################################
# Simple Object Kernel project Makefile #
# by Damien Dejean <djod4556@yahoo.fr>  #
#########################################

# Kernel configurations
include kernel/build/kernel-configs.mk
.DEFAULT_GOAL := $(KERNEL_DEFAULT)

# Copy the kernel binary to a disk image an run bochs
bochs: update-disk
	bochs

# Boot the kernel with qemu multiboot loader
qemu: $(KERNEL_DEFAULT)
	qemu -kernel kernel/$<

# Run qemu with a debug console using embedded multiboot loader
qemu-debug: $(KERNEL_QEMU_DEBUG)
	qemu -kernel kernel/$< -debugcon stdio

# Run qemu with a disk image and GRUB legacy as bootloader
qemu-disk: update-disk
	qemu -hda disk.img

# Update disk image with new compiled kernel
.PHONY: update-disk
update-disk: disk $(KERNEL_DEFAULT)
	@echo "### This target will require root access to mont disk image ! ###"
	sudo mount -t ext2 -o loop,offset=1048576 disk.img disk/
	sudo cp kernel/$(KERNEL_DEFAULT) disk/
	sync
	sudo umount disk/

disk:
	mkdir disk/

.PHONY: $(KERNEL_CONFIGS)
$(KERNEL_CONFIGS):
	make -C kernel/ $@

.PHONY: clean
clean:
	make -C kernel/ clean

