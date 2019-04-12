#include "client.h"

int main(int argc, char** argv) {
  // declare some variables
  int sock;
  struct sockaddr_in serverAddress;
  unsigned short serverPort;
  char* serverIP;
  char* filename;
  char rcvBuffer[RECV_BUF_SIZE];
  unsigned int filenameLength;
  int bytesReceived, totalBytesReceived;

  // Check cli input number
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <server IP> <server port> <filename>\n", argv[0]);
    exit(1);
  }

  // get input parameters
  serverIP = argv[1];
  serverPort = atoi(argv[2]);
  filename = argv[3];

  // establish socket
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throwError("socket() failed");
  }

  // set up serverAddress struct
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(serverIP);
  serverAddress.sin_port = htons(serverPort);

  // connect to server
  if (connect(sock, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
    throwError("connect() failed");
  }
  printf("Connection succeeded\n");

  // send filename to server
  filenameLength = strlen(filename);
  if (send(sock, filename, filenameLength, 0) != filenameLength) {
    throwError("send() did not send correct length message");
  }
  printf("sent message\n");

  // receive status of access() on server
  char fileExists;
  if (bytesReceived = recv(sock, &fileExists, 1, 0) <= 0) {
    throwError("Did not receive file access acknowledgement");
  }

  // check if file exists on server
  if (0 == fileExists) {
    throwError("Remote file does not exist");
  }

  // receive file
  int downloadedFile = open(filename, O_WRONLY | O_CREAT);
  if (-1 == downloadedFile) {
    throwError("Failed to access destination file");
  }

  printf("Receiving file\n");
  while ((bytesReceived = recv(sock, rcvBuffer, RECV_BUF_SIZE, 0)) > 0) {
    int writeResult = write(downloadedFile, rcvBuffer, bytesReceived);
  }

  /*totalBytesReceived = 0;*/

  /*while (1) {*/
    /*if ((bytesReceived = recv(sock, filename, RECV_BUF_SIZE-1, 0)) <= 0) {*/
      /*throwError("recv() failed or connection closed");*/
    /*}*/
    /*printf("Received %d bytes\n", bytesReceived);*/
    /*totalBytesReceived += bytesReceived;*/
    /*filename[bytesReceived] = '\0';*/
    /*printf("%s\n", filename);*/
  /*}*/
  /*printf("\n");*/
  close(sock);

  return 0;
}
