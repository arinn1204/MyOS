IMAGE_DIR 		:= Image
IMAGE 			:= $(IMAGE_DIR)/mtximage
IMAGE_CONTENTS 	:= $(IMAGE_DIR)/Contents
BOOT_DIR 		:= Bootloader
KERNEL_DIR		:= OS
USER_DIR 		:= User

BOOT 	:= boot
KERNEL 	:= mtx0
USER1 	:= u1
USER2	:= u2

CC		:= bcc
AS		:= as86
LD		:= ld86

CFLAGS	:= -ansi
LDFLAGS	:= -d /usr/lib/bcc/libc.a


# bootloader files
CORE_BOOT_FILES	:= bs.s main.c
BOOT_FILES 		:= $(addprefix $(BOOT_DIR)/, $(CORE_BOOT_FILES) io.c)
BOOT_OBJECTS 	:= $(patsubst %.c,%.o,$(patsubst %.s,%.asmo,$(BOOT_FILES) ) ) #.c -> .o and .s -> .asmo

# Kernel files
CORE_KERNEL_FILES 	:= $(addprefix $(KERNEL_DIR)/, start.s main.c)
KERNEL_C_FILES 		:= $(addprefix $(KERNEL_DIR)/, fs.c int.c io.c kernel.c proc.c queue.c wait.c)
KERNEL_S_FILES		:= $(addprefix $(KERNEL_DIR)/, tswitch.s )#reg.s screen.s)
KERNEL_OBJECTS 		:= $(patsubst %.c,%.o,$(patsubst %.s,%.asmo,$(CORE_KERNEL_FILES) ) )
KERNEL_OBJECTS		+= $(patsubst %.c,%.o,$(KERNEL_C_FILES)) $(patsubst %.s,%.asmo,$(KERNEL_S_FILES))


#User files
CORE_USER1_FILES	:= $(addprefix $(USER_DIR)/, userstart.s user.c)
USER1_C_FILES		:= $(addprefix $(USER_DIR)/, usercode.c)
USER1_OBJECTS		:= $(patsubst %.c,%.o,$(patsubst %.s,%.asmo,$(CORE_USER1_FILES) ) )
USER1_OBJECTS 		+= $(patsubst %.c,%.o,$(USER1_C_FILES))


all: boot
all: kernel user1
	@echo Copying bootloader,kernel, and user into image
	dd if=$(BOOT_DIR)/$(BOOT) of=$(IMAGE) bs=1024 count=1 conv=notrunc
	sudo mount -o loop $(IMAGE) $(IMAGE_CONTENTS)
	sudo cp $(KERNEL_DIR)/$(KERNEL) $(IMAGE_CONTENTS)/boot/$(KERNEL)
	sudo cp $(USER_DIR)/$(USER1) $(IMAGE_CONTENTS)/bin/$(USER1)
	sudo umount $(IMAGE_CONTENTS)
	@echo ""
	@echo Complete...

run: all
	qemu -fda $(IMAGE) -no-fd-bootchk


boot: CFLAGS = -ansi -D_BOOT_
boot: $(BOOT_OBJECTS)
	@echo Linking the bootloader together.....
	$(LD) $^ $(LDFLAGS) -o $(BOOT_DIR)/$(BOOT)
	@echo Bootloader complete
	@echo ""
	@echo ""


kernel: CFLAGS = -ansi -D_LAB_3_ -D_MTXLIB_
kernel: INCLIB := $(KERNEL_DIR)/mtxlib
kernel: GETSIZE := stat $(KERNEL_DIR)/$(KERNEL) | grep Size | awk '{print $$2}'
kernel: $(KERNEL_OBJECTS)
	@echo Linking the kernel together......
	$(LD) $^  $(INCLIB) $(LDFLAGS) -o $(KERNEL_DIR)/$(KERNEL)
	@echo Kernel complete
	@printf "KERNEL Image is (in bytes): " && $(GETSIZE)
	@echo ""
	@echo ""

user1: CFLAGS = -ansi -D_LAB_3_
user1: LDFLAGS = /usr/lib/bcc/libc.a
user1: INCLIB := $(USER_DIR)/mtxlib
user1: $(USER1_OBJECTS)
	@echo Linking U1 together.....
	$(LD) $^ $(INCLIB) $(LDFLAGS) -o $(USER_DIR)/$(USER1)
	@echo U1 complete.
	@echo ""
	@echo ""



%.asmo: %.s
	$(AS) -o $(patsubst %.s,%.asmo,$<) $<

clean:
	$(RM) $(BOOT_OBJECTS)
	$(RM) $(KERNEL_OBJECTS)
	$(RM) $(USER1_OBJECTS)



cleanall: clean
	$(RM) $(BOOT_DIR)/$(BOOT) $(KERNEL_DIR)/$(KERNEL) $(USER_DIR)/$(USER1)





