#include "client.h"

int main(int argc, char** argv) {
  // declare some variables
  int socket;
  struct sockaddr_in serverAddress;
  unsigned short serverPort;
  char* serverIP;
  char* filename;
  char fileBuffer[RECV_BUF_SIZE];
  unsigned int filenameLength;
  int bytesReceived, totalBytesReceived;

  // Check cli input number
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <server IP> <server port> <filename>\n", argv[0]);
    exit(1);
  }

  serverIP = argv[1];

  // set up serverAddress struct
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(serverIP);
  serverAddress.sin_port = htons(serverPort);

  if (connect(socket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
    throwError("connect() failed");
  }

  printf("Connection succeeded\n");
  filenameLength = strlen(filename);

  if (send(socket, filename, filenameLength, 0) != filenameLength) {
    throwError("send() did not send correct length message");
  }

  printf("sent message\n");

  totalBytesReceived = 0;

  while (totalBytesReceived < filenameLength) {
    if ((bytesReceived = recv(socket, filename, RECV_BUF_SIZE-1, 0)) <= 0) {
      throwError("recv() failed or connection closed");
    }
    printf("Received %d bytes\n", bytesReceived);
    totalBytesReceived += bytesReceived;
    filename[bytesReceived] = '\0';
    printf(fileBuffer);
  }
  printf("\n");
  close(socket);

  return 0;
}
