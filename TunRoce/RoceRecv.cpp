// RoceRecv.cpp: receive RoCEv2 packet sended by tun device
#include "Tun.h"
#define PORT 4791

int main() {
  int sockfd;
  unsigned char buffer[PKT_LEN];
  char sendbuffer[PKT_LEN];
  unsigned char example[PKT_LEN];
  struct sockaddr_in servaddr, cliaddr;
  char name[FILE_NAME_LEN];

  // Preparation for receiving packets
  int n;
  socklen_t len = sockPre(sockfd, servaddr, cliaddr, PORT);

  for (;;) {
    // Receive RoCEv2 packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sockfd, buffer, PKT_LEN, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr,
                 &len);  //接收到说明IP包格式和UDP格式正确
    printf("received %d bytes, success\n", n);
  }

  return 0;
}