#include "common.h"

void throwError(char* msg) {
  throwErrorWithCode(msg, 1);
}

void throwErrorWithCode(char* msg, int exitCode) {
  perror(msg);
  exit(exitCode);
}

unsigned char checksum(char* fileByte, size_t size, unsigned char check) {
  while (0 != size--) {
    check -= *fileByte++;
  }
  return check;
}
