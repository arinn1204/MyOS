CC := bcc
AS := as86
LD := ld86
RM := rm -f
CFLAGS := -ansi
EXEC := u1
LDFLAGS := /usr/lib/bcc/libc.a
COBJECTS := user.o usercode.o print.o
OBJECTS := suser.o $(COBJECTS)


all: $(COBJECTS)
	$(AS) -o suser.o user.s
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(EXEC)

clean:
	$(RM) $(OBJECTS) $(EXEC)