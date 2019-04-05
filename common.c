#include "common.h"

void throwError(char* msg) {
  throwErrorWithCode(msg, 1);
}

void throwErrorWithCode(char* msg, int exitCode) {
  perror(msg);
  exit(exitCode);
}

