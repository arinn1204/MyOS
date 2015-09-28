ROOTDIR := /home/aaron/MyOS
IMAGEDIR := $(ROOTDIR)/Image
IMAGE := $(IMAGEDIR)/mtximage
IMAGE_CONTENTS := $(IMAGEDIR)/Contents
BOOT := $(ROOTDIR)/Bootloader
OS := $(ROOTDIR)/OS
USER := $(ROOTDIR)/User



all:
	@clear
	@echo "Making bootloader..."
	$(MAKE) -C $(BOOT)
	@echo "Bootloader done"
	@echo ""
	@echo ""
	@echo "Making the kernel image..."
	$(MAKE) -C $(OS)
	@echo "Kernel image is complete"
	@echo ""
	@echo ""
	@echo "Making the user image..."
	$(MAKE) -C $(USER)
	@echo "User image is complete"
	@echo ""
	@echo ""

check: all
	@test -n "$$(find $(BOOT)/boot -size -1025c)"
	@echo "File size is correct it is less than 1024 bytes"

run: check
	@echo ""
	@echo ""
	dd if=$(BOOT)/boot of=$(IMAGE) bs=1024 count=1 conv=notrunc
	@sudo mount -o loop $(IMAGE) $(IMAGE_CONTENTS)
	sudo cp $(OS)/mtx0 $(IMAGE_CONTENTS)/boot/mtx0
	sudo cp $(USER)/u1 $(IMAGE_CONTENTS)/bin/u1
	@sudo umount $(IMAGE_CONTENTS)
	qemu -fda $(IMAGE) -no-fd-bootchk

clean:
	$(MAKE) -C $(BOOT) clean
	@echo ""
	@echo ""
	$(MAKE) -C $(OS) clean
	@echo ""
	@echo ""
	$(MAKE) -C $(USER) clean

