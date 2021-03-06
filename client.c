#include "client.h"

int main(int argc, char** argv) {
  // declare some variables
  int sock;
  struct sockaddr_in serverAddress;
  unsigned short serverPort;
  struct sockaddr_in fromAddress;
  unsigned int fromSize;
  char* serverIP;
  char* filename;
  char* end = '\0';
  char rcvBuffer[RECV_BUF_SIZE];
  unsigned int filenameLength;
  int bytesReceived, totalBytesReceived = 0;
  end;

  // Check cli input number
  if (argc != 4) {
    fprintf(stderr, "[CLIENT] Usage: %s <server IP> <server port> <filename>\n", argv[0]);
    exit(1);
  }

  // get input parameters
  serverIP = argv[1];
  serverPort = atoi(argv[2]);
  filename = argv[3];

  // establish socket
  if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    throwError("socket() failed");
  }

  // set up serverAddress struct
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(serverIP);
  serverAddress.sin_port = htons(serverPort);

  do {
    totalBytesReceived = 0;
    // send filename to server
    filenameLength = strlen(filename);
    if (sendto(
          sock,
          filename,
          filenameLength,
          0,
          (struct sockaddr*) &serverAddress,
          sizeof(serverAddress))
        != filenameLength) {
      throwError("send() did not send correct length message");
    }
    printf("[CLIENT] sent message\n");

    // receive status of access() on server
    int fileSize;
    fromSize = sizeof(fromAddress);
    if ((bytesReceived = recvfrom(
            sock,
            &fileSize,
            sizeof(int),
            0,
            (struct sockaddr*) &fromAddress,
            &fromSize))
        <= 0) {
      throwError("Did not receive file access acknowledgement");
    }

    if (serverAddress.sin_addr.s_addr != fromAddress.sin_addr.s_addr) {
      fprintf(stderr,"[CLIENT] Error: received a packet from unknown source.\n");
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

    printf("[CLIENT] Receiving file\n");
    unsigned char check = 0;
    unsigned char packetCheck;
    unsigned int seqnum = 0;
    int nextseqnum = 0;
    // receive file and calculate checksum
    while (totalBytesReceived < fileSize) {
      bytesReceived = recv(sock, rcvBuffer, RECV_BUF_SIZE, 0);
      // reassemble sequence number
      seqnum =
        ((unsigned char) rcvBuffer[0]) << 24 |
        ((unsigned char) rcvBuffer[1]) << 16 |
        ((unsigned char) rcvBuffer[2]) << 8 |
        ((unsigned char) rcvBuffer[3]);
      // check packet doesn't have flipped bits
      packetCheck = 0;
      packetCheck = checksum(
          rcvBuffer+sizeof(seqnum),
          bytesReceived-sizeof(seqnum),
          packetCheck);
      char sendBuffer[sizeof(nextseqnum)];
      if (0 == packetCheck) {
        // inform the user
        printf("[CLIENT] good packetCheck: %s\n", rcvBuffer+sizeof(seqnum));
        // inform the server
        totalBytesReceived += bytesReceived - sizeof(seqnum) - sizeof(packetCheck);
        nextseqnum = totalBytesReceived + 1;
        // set next seqnum to request
        sendBuffer[0] = nextseqnum >> 24;
        sendBuffer[1] = nextseqnum >> 16;
        sendBuffer[2] = nextseqnum >> 8;
        sendBuffer[3] = nextseqnum - 1;
        if (sendto(
              sock,
              sendBuffer,
              sizeof(nextseqnum),
              0,
              (struct sockaddr*) &serverAddress,
              sizeof(serverAddress))
            != sizeof(nextseqnum)) {
          printf("[CLIENT] failed to send ACK\n");
        }
        // account for file-ending checksum
        if (totalBytesReceived > fileSize) {
          bytesReceived--;
          check = checksum(rcvBuffer+bytesReceived+sizeof(seqnum), 1, check);
        }
        // go to location of given bytes
        lseek(downloadedFile, seqnum, SEEK_SET);
        // write data to file
        int writeResult = write(
            downloadedFile,
            rcvBuffer+sizeof(seqnum),
            bytesReceived - sizeof(seqnum) - sizeof(packetCheck));
        check = checksum(rcvBuffer+sizeof(seqnum), bytesReceived-sizeof(seqnum), check);
      } else {
        // use same seqnum again
        sendBuffer[0] = nextseqnum >> 24;
        sendBuffer[1] = nextseqnum >> 16;
        sendBuffer[2] = nextseqnum >> 8;
        sendBuffer[3] = nextseqnum - 1;
        // inform the user
        printf("[CLIENT] bad packet, requesting resend\n");
        // inform the server
        if (sendto(
              sock,
              sendBuffer,
              sizeof(nextseqnum),
              0,
              (struct sockaddr*) &serverAddress,
              sizeof(serverAddress))
            != sizeof(nextseqnum)) {
          printf("[CLIENT] failed to send NAK\n");
        }
      }
    }

    // close file
    close(downloadedFile);

    // check if checksums match
    if (0 != check) {
      bytesReceived = recv(sock, rcvBuffer, RECV_BUF_SIZE, 0);
      check = checksum(rcvBuffer, 1, check);
      if (0 != check) {
        fprintf(stderr, "[CLIENT] The file received does not have a matching checksum\n");
      }
    }

    // prompt for additional files
    char newFilename[100];
    printf("[CLIENT] Enter another filename to download or type \"exit\"\n");
    scanf("%s", newFilename);
    filename = newFilename;

    recv(sock, NULL, RECV_BUF_SIZE, MSG_DONTWAIT);
    end;
  } while (strcmp(filename, "exit") != 0);

  close(sock);

  return 0;
}
