#include "server.h"

int main(int argc, char** argv) {
  // defining some variables
  int serverSocket;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  unsigned int serverPort;
  unsigned int clientLength;
  char filename[RECV_BUF_SIZE];
  int msgSize;

  // check that server port was provided
  if (argc != 2) {
    fprintf(stderr, "[SERVER] Usage: %s <server port number>\n", argv[0]);
    exit(1);
  }

  serverPort = atoi(argv[1]); // set server port

  // create UDP socket
  if ((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    throwError("socket() failed");
  }

  // set up serverAddress struct
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(serverPort);

  // bind socket
  if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
    throwError("bind() failed");
  } else {
    printf("[SERVER] bind() succeeded\n");
  }

  for (;;) {

    printf("[SERVER] Handling request from %s\n", inet_ntoa(clientAddress.sin_addr));
    if ((msgSize = recvfrom(
            serverSocket,
            filename,
            RECV_BUF_SIZE-1,
            0,
            (struct sockaddr*) &clientAddress,
            &clientLength))
        < 0) {
      throwError("recvfrom() failed");
    }
    filename[msgSize] = '\0';

    printf("[SERVER] Message size: %d\n", msgSize);
    printf("[SERVER] Filename: %s\n", filename);

    // check if file exists
    char fileExists = (char) access(filename, F_OK);
    int fileSize;
    if (0 != fileExists) {
      printf("[SERVER] File does not exist\n");
      fileSize = 0;
    } else {
      // get size of file
      struct stat st;
      stat(filename, &st);
      fileSize = st.st_size;
    }

    printf("[SERVER] Client address: %d\n", clientAddress);

    // send fileSize
    if (sendto(
          serverSocket,
          &fileSize,
          sizeof(int),
          0,
          (struct sockaddr*) &clientAddress,
          sizeof(clientAddress))
        != sizeof(int)) {
      printf("[SERVER] Failed to send file size\n");
    } else {
      // open file
      int file = open(filename, O_RDONLY);
      if (file < 0) {
        printf("[SERVER] Failed to open file\n");
      }
      printf("[SERVER] Sending file %s\n", filename);
      int readResult, sendResult;
      char sendBuffer[BUF_SIZE];
      int sendTotal = 0;
      unsigned char check = 0;
      // send file to client
      while (sendTotal < fileSize){
        readResult = read(file, sendBuffer, BUF_SIZE);
        check = checksum(sendBuffer, readResult, check);
        sendResult = sendto(
            serverSocket,
            sendBuffer,
            readResult,
            0,
            (struct sockaddr*) &clientAddress,
            sizeof(clientAddress));
        sendTotal += sendResult;
      }

      // send checksum of file
      sendBuffer[0] = check;
      sendResult = sendto(
          serverSocket,
          sendBuffer,
          sizeof(char),
          0,
          (struct sockaddr*) &clientAddress,
          sizeof(clientAddress));

      printf("[SERVER] Done sending file %s\n", filename);
    }
 }

  return 0;
}

