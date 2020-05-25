CC=gcc
DEBUG=-g -O0
CFLAGS=$(DEBUG)

all: shell.x

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

shell.x: shell.o
	$(CC) -o $@ $<

clean:
	rm -f *.o shell.x \#*\# *~
