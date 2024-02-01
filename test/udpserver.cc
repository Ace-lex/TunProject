// Receive the packets sended by tun and check the correctness

#include <sys/time.h>

#include "tun.h"

const int kNumTestFile = 3;    // The number of test cases
const int kRecvPort = 8080;    // The receiving port of socket
const int kPacketLen = 4096;   // The max length of packet length
const int kFileNameLen = 100;  // The max length of filename
const char *const kTestFilePrefix =
    "./testfile/test";  // The script path to configure tun device
const char *const kTestFileSuffix = ".bin";  // The testfile suffix
const int kWaitTime = 10;                    // The timeout of test

void RecvAndCheck(int sock_fd, struct sockaddr_in *client_addr, socklen_t len) {
  uint8_t example[kPacketLen];
  uint8_t buffer[kPacketLen];
  char name[kFileNameLen];
  int n;
  for (int i = 0; i < kNumTestFile; i++) {
    // Receive the packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sock_fd, buffer, kPacketLen, MSG_WAITALL,
                 (struct sockaddr *)client_addr,
                 &len);  // Receiving packets successfully indicates that the IP
                         // and UDP packet formats are correct.

    // Read the check file
    sprintf(name, "%s%d%s", kTestFilePrefix, i, kTestFileSuffix);
    FILE *fp = fopen(name, "rb");
    int file_length;
    file_length = FileSize(name);
    fread(example, 1, file_length, fp);
    fclose(fp);

    // Check length correction
    assert(n == file_length);

    // Check payload correction
    for (int j = 0; j < file_length; j++) {
      assert(example[j] == buffer[j]);
    }

    printf("Received %d bytes, test%d passed\n", n, i);
  }
}

int main() {
  int sock_fd;
  struct timeval tv;
  tv.tv_sec = kWaitTime;
  tv.tv_usec = 0;
  uint8_t buffer[kPacketLen];

  struct sockaddr_in server_addr, client_addr;
  char name[kFileNameLen];
  int n;

  // Preparation for receiving
  socklen_t len = SockPrepare(&sock_fd, &server_addr, &client_addr, kRecvPort);
  // Set the timeout of test
  setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  // Receive packets from tun device and check UDPTunSend and UDPTunSendv2's
  // correctness
  printf("====UDPTunSend(use memcpy)====\n");
  fflush(stdout);
  RecvAndCheck(sock_fd, &client_addr, len);

  printf("====UDPTunSend(use writev)====\n");
  fflush(stdout);
  RecvAndCheck(sock_fd, &client_addr, len);

  return 0;
}