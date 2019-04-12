CC=gcc
CFLAGS=-Wall -g

all: client server

SRC = common.c

client: client.c $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

server: server.c $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o client server

