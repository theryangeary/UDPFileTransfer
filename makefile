CC=gcc
CFLAGS=-Wall

all: client server

OBJ = DieWithError.o HandleTCPClient.o

client: TCPEchoClient.o $(OBJ)
	$(CC) -o $@ $^

server: TCPEchoServer.o $(OBJ)
	$(CC) -o $@ $^

clean:
	rm *.o client server

