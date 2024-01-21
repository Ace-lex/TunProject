// RoceRecv.cpp: receive RoCEv2 packet sended by tun device

#include "Tun.h"

#define PORT 4791
#define PKT_LEN 4096
#define FILE_NAME_LEN 100

int main() {
  int sockfd;
  uint8_t buffer[PKT_LEN];
  char sendbuffer[PKT_LEN];
  uint8_t example[PKT_LEN];
  struct sockaddr_in servaddr, cliaddr;
  char name[FILE_NAME_LEN];

  // Preparation for receiving packets
  int n;
  socklen_t len = sockPre(&sockfd, &servaddr, &cliaddr, PORT);

  while (true) {
    // Receive RoCEv2 packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sockfd, buffer, PKT_LEN, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr, &len);
    printf("received %d bytes, success\n", n);
  }

  return 0;
}