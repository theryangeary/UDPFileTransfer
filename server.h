#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 32

void handleClient(int socket);
