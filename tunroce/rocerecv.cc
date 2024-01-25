// RoceRecv.cpp: receive RoCEv2 packet sended by tun device

#include "tun.h"

const int kRecvPort = 4791;   // Receiving port of the socket
const int kPacketLen = 4096;  //  The max length of packet length

int main() {
  int sock_fd;
  uint8_t buffer[kPacketLen];
  uint8_t example[kPacketLen];
  struct sockaddr_in server_addr, client_addr;

  // Preparation for receiving packets
  int n;
  socklen_t len = SockPrepare(&sock_fd, &server_addr, &client_addr, kRecvPort);

  while (true) {
    // Receive RoCEv2 packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sock_fd, buffer, kPacketLen, MSG_WAITALL,
                 (struct sockaddr *)&client_addr, &len);
    printf("received %d bytes, success\n", n);
  }

  return 0;
}