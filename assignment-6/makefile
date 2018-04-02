CC = gcc
CFLAGS = -Wall -O0 -std=gnu99 -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -pthread -lm -lwiringPi
FUSEFLAGS = `pkg-config fuse --cflags --libs`

SOURCES = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SOURCES))
BINS = $(SOURCES:.c=)

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(FUSEFLAGS) $<

clean:
	$(RM) *.o *.a $(BINS)