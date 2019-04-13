#include "server.h"

int main(int argc, char** argv) {
  // defining some variables
  int serverSocket;
  int clientSocket;
  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;
  unsigned int serverPort;
  unsigned int clientLength;

  // check that server port was provided
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <server port number>\n", argv[0]);
    exit(1);
  }

  serverPort = atoi(argv[1]); // set server port

  // create TCP socket
  if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
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
    printf("bind() succeeded\n");
  }

  // listen for incoming connections
  if (listen(serverSocket, MAXSOCK) < 0) {
    throwError("listen() failed");
  } else {
    printf("listen() succeeded\n");
  }

  while (1) {
    clientLength = sizeof(clientAddress);
    if ((clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientLength)) < 0) {
      throwError("accept() failed");
    }
    printf("Serving client %s\n", inet_ntoa(clientAddress.sin_addr));
    handleClient(clientSocket);
  }

  return 0;
}

void handleClient(int sock) {
  char filename[RECV_BUF_SIZE];
  int msgSize;

  printf("Handling sock %d\n", sock);

  if ((msgSize = recv(sock, filename, RECV_BUF_SIZE, 0)) < 0) {
    throwError("recv() failed");
  }
  filename[msgSize] = '\0';

  printf("Message size: %d\n", msgSize);

  // check if file exists
  char fileExists = (char) access(filename, F_OK);
  int fileSize;
  if (0 != fileExists) {
    printf("File does not exist\n");
    fileSize = 0;
  } else {
    // get size of file
    struct stat st;
    stat(filename, &st);
    fileSize = st.st_size;
  }

  // send fileSize
  if (send(sock, &fileSize, sizeof(int), 0) != sizeof(int)) {
    printf("Failed to send file size\n");
  } else {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
      printf("Failed to open file\n");
    }
    printf("Sending file %s\n", filename);
    int readResult, sendResult;
    char sendBuffer[BUF_SIZE];
    int sendTotal = 0;
    while (sendTotal < fileSize){
      readResult = read(file, sendBuffer, BUF_SIZE);
      sendResult = send(sock, sendBuffer, readResult, 0);
      sendTotal += sendResult;
    }
  }
}
