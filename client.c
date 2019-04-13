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
  int bytesReceived, totalBytesReceived = 0;

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

  do {
    // send filename to server
    filenameLength = strlen(filename);
    printf("%d\n", filenameLength);
    if (send(sock, filename, filenameLength, 0) != filenameLength) {
      throwError("send() did not send correct length message");
    }
    printf("sent message\n");

    // receive status of access() on server
    int fileSize;
    if ((bytesReceived = recv(sock, &fileSize, sizeof(int), 0)) <= 0) {
      throwError("Did not receive file access acknowledgement");
    }

    // check if file exists on server
    if (0 == fileSize) {
      errno = ENOENT;
      throwError("Remote file does not exist");
    }

    // receive file
    int downloadedFile = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
    if (-1 == downloadedFile) {
      throwError("Failed to access destination file");
    }

    printf("Receiving file\n");
    while (totalBytesReceived < fileSize) {
      bytesReceived = recv(sock, rcvBuffer, RECV_BUF_SIZE, 0);
      int writeResult = write(downloadedFile, rcvBuffer, bytesReceived);
      totalBytesReceived += bytesReceived;
    }
    close(downloadedFile);

    char newFilename[100];
    printf("Enter another filename to download or type \"exit\"\n");
    scanf("%s", newFilename);
    filename = newFilename;
  } while (strcmp(filename, "exit") != 0);

  close(sock);

  return 0;
}
