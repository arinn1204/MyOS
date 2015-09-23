CC := bcc
LD := ld86
AS := as86
RM := rm -f
EXECUTABLE := mtx0
GETSIZE := stat $(EXECUTABLE) | grep Size | awk '{print $$2}'

CFLAGS := -ansi
PROC_OBJECTS := main.o fs.o kernel.o int.o io.o proc.o queue.o wait.o
OBJECTS := sproc.o $(PROC_OBJECTS)
LDFLAGS := -d /usr/lib/bcc/libc.a

all: $(PROC_OBJECTS)
	$(AS) sproc.s -o sproc.o
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)
	@printf "KERNEL Image is (in bytes): " && $(GETSIZE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
