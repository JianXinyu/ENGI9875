# CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra

filesystem: fat16.o fat16_fuse.o
	$(CC) $(CFLAGS) -o filesystem fat16.o fat16_fuse.o $(shell pkg-config fuse --cflags --libs)

fat16.o: fat16.c fat16.h

fat16_fuse.o: fat16_fuse.c fat16_fuse.h
	$(CC) $(shell pkg-config fuse --cflags --libs) fat16_fuse.c -c

clean:
	$(RM) filesystem *.o

.PHONY: clean

# CFLAGS=	-g -Wall `pkg-config --cflags fuse` \
# 	-D USE_EXAMPLE \
# 	-fsanitize=address -fno-omit-frame-pointer

# LDFLAGS=`pkg-config --libs fuse` -fsanitize=address

# OBJS=	\
# 	assign4.o \
# 	fuse.o \

# all: fuse
# fuse: ${OBJS}
# 	${CC} ${OBJS} ${LDFLAGS} -o fuse

# # Express header dependencies: recompile these object files if header changes
# fuse.o: assign4.h
# assign4.o: assign4.h

# clean:
# 	rm -f fuse *.o