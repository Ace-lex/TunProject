// RoceRecv.cpp: receive RoCEv2 packet sended by tun device

#include "tun.h"

#define PORT 4791
const int kFileNameLen = 100;
const int kPacketLen = 4096;

int main() {
  int sockfd;
  uint8_t buffer[kPacketLen];
  char sendbuffer[kPacketLen];
  uint8_t example[kPacketLen];
  struct sockaddr_in servaddr, cliaddr;
  char name[kFileNameLen];

  // Preparation for receiving packets
  int n;
  socklen_t len = SockPrepare(&sockfd, &servaddr, &cliaddr, PORT);

  while (true) {
    // Receive RoCEv2 packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sockfd, buffer, kPacketLen, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr, &len);
    printf("received %d bytes, success\n", n);
  }

  return 0;
}