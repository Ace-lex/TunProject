// NormalDevice.cpp: communicate with the TUN device.
// Usage: ./NormalDevice [filename]

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024
#define DST_IP "10.10.10.1"

int fileSize(char *filename) {
  struct stat statbuf;
  stat(filename, &statbuf);
  int size = statbuf.st_size;

  return size;
}

int main(int argc, char *argv[]) {
  int sockfd;
  char buffer[MAXLINE];
  struct sockaddr_in servaddr;
  int payloadLen;

  // Create the socket file description
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  // printf("socket successfully built\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // Populate the server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr(DST_IP);

  socklen_t len = (socklen_t)sizeof(servaddr);  // len is value/resuslt

  // Send message
  while (true) {
    // Read payload file or get payload from stdin
    if (argc == 1) {
      printf("input payload: ");
      fgets(buffer, MAXLINE, stdin);
      payloadLen = strlen(buffer);
      buffer[strcspn(buffer, "\n")] = 0;
    } else {
      FILE *fp = fopen(argv[1], "rb");
      payloadLen = fileSize(argv[1]);
      fread(buffer, 1, payloadLen, fp);
      fclose(fp);
    }

    // Send packets to tun device
    sendto(sockfd, (const char *)buffer, payloadLen, 0,
           (const struct sockaddr *)&servaddr, len);
    fprintf(stdout, "message: %s have sent.\n", buffer);
    fflush(stdout);

    // Receive packets from tun device
    int n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                     (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
  }

  close(sockfd);
  return 0;
}