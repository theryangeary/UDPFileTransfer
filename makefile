CC=gcc
CFLAGS=-Wall

all: client server

OBJ = common.o

client: client.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

server: server.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o client server

