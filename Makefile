ROOTDIR := /home/aaron/MyOS
IMAGEDIR := $(ROOTDIR)/Image
IMAGE := $(IMAGEDIR)/mtximage
BOOT := $(ROOTDIR)/Bootloader
OS := $(ROOTDIR)/Proc


all:
	$(MAKE) -C $(BOOT)
	$(MAKE) -C $(OS)

check: all
	@test -n "$$(find $(BOOT)/boot -size -1025c)"
	@echo "File size is correct it is less than 1024 bytes"

run: check
	dd if=$(BOOT)/boot of=$(IMAGE) bs=1024 count=1 conv=notrunc
	@sudo mount -o loop $(IMAGE) $(ROOTDIR)/OS
	sudo cp $(OS)/mtx0 $(ROOTDIR)/OS/boot/mtx0
	@sudo umount $(ROOTDIR)/OS
	qemu -fda $(IMAGE) -no-fd-bootchk

clean:
	$(MAKE) -C $(BOOT) clean
	$(MAKE) -C $(OS) clean

