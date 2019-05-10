#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <execinfo.h>

#define MAXSOCK 5
#define RECV_BUF_SIZE 32
#define SIZE 100

void throwError(char* msg);
void throwErrorWithCode(char* msg, int exitCode);
unsigned char checksum(char* fileByte, size_t size, unsigned char check);
void printCallingExecutable();
