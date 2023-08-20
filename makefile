.PHONY: clean

CC=gcc
CFLAGS=-Wall

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clientelf: client.o
	$(CC) -o $@ $<

clean:
	rm -f *.o clientelf

