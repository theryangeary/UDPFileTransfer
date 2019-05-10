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
  /*printCallingExecutable();*/
  /*printf(": Checksum value: %02x\n", check);*/
  return check;
}

void printCallingExecutable() {
  int nptrs;
  void *buffer[SIZE];
  char **strings;
  nptrs = backtrace(buffer, SIZE);
  strings = backtrace_symbols(buffer, nptrs);
  printf("%s", strings[0]);
  free(strings);
}
