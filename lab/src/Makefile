CC=	clang
CFLAGS=	-Weverything -g

all: syscalls
clean:
	rm -f syscalls *.o

syscalls: main.o stuff_64.o
	$(CC) main.o stuff_64.o -o syscalls

