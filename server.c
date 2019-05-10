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
  int windowSize;
  float errorProbability;

  // check that server port and window size was provided
  if (argc != 4) {
    fprintf(stderr, "[SERVER] Usage: %s <server port number>\n", argv[0]);
    exit(1);
  }

  serverPort = atoi(argv[1]); // set server port
  windowSize = atoi(argv[2]); // set window size
  errorProbability = atof(argv[3]); // set probability of intentional errors

  // create UDP socket
  if ((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    throwError("socket() failed");
  }

  // set up serverAddress struct
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(serverPort);

  srand(time(NULL));

  // bind socket
  if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
    throwError("bind() failed");
  } else {
    printf("[SERVER] bind() succeeded\n");
  }

  clientLength = sizeof(clientAddress);

  for (;;) {

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

    printf("[SERVER] Handling request from %s\n", inet_ntoa(clientAddress.sin_addr));
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

    // send fileSize
    if (sendto(
          serverSocket,
          &fileSize,
          sizeof(int),
          0,
          (struct sockaddr*) &clientAddress,
          sizeof(clientAddress))
        != sizeof(int)) {
      perror("[SERVER] Failed to send file size");
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
      unsigned char packetCheck = 0;
      // GBN setup
      int base = 0;
      int nextseqnum = 0;
      // send file to client
      while (nextseqnum < fileSize){
        // set send buffer with:
        // - seqnum
        sendBuffer[0] = (char) nextseqnum;
        // - data
        readResult = read(
            file,
            sendBuffer+sizeof(nextseqnum),
            BUF_SIZE-sizeof(packetCheck)-1);
        // - checksum
        packetCheck = checksum(sendBuffer, readResult, packetCheck);
        sendBuffer[BUF_SIZE-1] = packetCheck;
        // track total checksum to make sure final file is correct later
        check = checksum(sendBuffer, readResult, check);
        // send to client
        sendResult = sendto(
            serverSocket,
            sendBuffer,
            readResult + sizeof(nextseqnum) + sizeof(packetCheck),
            0,
            (struct sockaddr*) &clientAddress,
            sizeof(clientAddress));
        nextseqnum = nextseqnum + readResult;
        /*sendTotal += sendResult;*/
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

