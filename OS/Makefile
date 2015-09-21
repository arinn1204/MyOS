CC := bcc
LD := ld86
AS := as86
RM := rm -f

EXECUTABLE := mtx0
CFLAGS := -ansi
PROC_OBJECTS := main.o kernel.o io.o proc.o queue.o wait.o
OBJECTS := sproc.o $(PROC_OBJECTS)
LDFLAGS := -d /usr/lib/bcc/libc.a

all: $(PROC_OBJECTS)
	$(AS) sproc.s -o sproc.o
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
