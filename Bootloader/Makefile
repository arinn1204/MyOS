OBJECT_FILES := main.o ../Shared/io.o
CFLAGS := -ansi -D_BOOT_
LDFLAGS := -d /usr/lib/bcc/libc.a
OBJECTS := bs.o $(OBJECT_FILES) 
CC := bcc 
LD := ld86
AS := as86
RM := rm -f
EXECUTABLE := boot

all: $(OBJECT_FILES)
	$(AS) -o bs.o bs.s
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)
clean:
	@$(RM) $(OBJECTS) $(EXECUTABLE)
