// UDPServer.cpp: receive udp packet sended by tun device and check for
// correctness

#include <sys/time.h>

#include "tun.h"

const int kNumTestFile = 3;
const int kListenPort = 8080;
const int kPacketLen = 4096;
const int kFileNameLen = 100;
const char *const kTestFilePrefix = "./testfile/test";
const char *const kTestFileSuffix = ".bin";
const int kWaitTime = 10;

int main() {
  int sock_fd;
  struct timeval tv;
  tv.tv_sec = kWaitTime;
  tv.tv_usec = 0;
  uint8_t buffer[kPacketLen];
  uint8_t example[kPacketLen];
  struct sockaddr_in server_addr, client_addr;
  char name[kFileNameLen];
  int n;

  // Preparation for receiving
  socklen_t len =
      SockPrepare(&sock_fd, &server_addr, &client_addr, kListenPort);
  setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  for (int i = 0; i < kNumTestFile; i++) {
    // Receive the packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sock_fd, buffer, kPacketLen, MSG_WAITALL,
                 (struct sockaddr *)&client_addr,
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

  return 0;
}