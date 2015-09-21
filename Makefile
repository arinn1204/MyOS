ROOTDIR := /home/aaron/MyOS
IMAGEDIR := $(ROOTDIR)/Image
IMAGE := $(IMAGEDIR)/mtximage
IMAGE_CONTENTS := $(IMAGEDIR)/Contents
BOOT := $(ROOTDIR)/Bootloader
OS := $(ROOTDIR)/OS



all:
	$(MAKE) -C $(BOOT)
	$(MAKE) -C $(OS)

check: all
	@test -n "$$(find $(BOOT)/boot -size -1025c)"
	@echo "File size is correct it is less than 1024 bytes"

run: check
	dd if=$(BOOT)/boot of=$(IMAGE) bs=1024 count=1 conv=notrunc
	@sudo mount -o loop $(IMAGE) $(IMAGE_CONTENTS)
	sudo cp $(OS)/mtx0 $(IMAGE_CONTENTS)/boot/mtx0
	@sudo umount $(IMAGE_CONTENTS)
	qemu -fda $(IMAGE) -no-fd-bootchk

clean:
	$(MAKE) -C $(BOOT) clean
	$(MAKE) -C $(OS) clean

