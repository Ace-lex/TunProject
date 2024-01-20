// UDPServer.cpp: receive udp packet sended by tun device and check for
// correctness

#include "Tun.h"

#define TEST_FILE 3
#define PORT 8080
#define TEST_FILE_PREFIX "./testfile/test"
#define TEST_FILE_SUFFIX ".bin"

int main() {
  int sockfd;
  unsigned char buffer[PKT_LEN];
  char sendbuffer[PKT_LEN];
  unsigned char example[PKT_LEN];
  struct sockaddr_in servaddr, cliaddr;
  char name[FILE_NAME_LEN];
  int n;

  // Preparation for receiving
  socklen_t len = sockPre(&sockfd, &servaddr, &cliaddr, PORT);

  for (int i = 0; i < TEST_FILE; i++) {
    // Receive the packets
    memset(example, 0, sizeof(example));
    n = recvfrom(sockfd, buffer, PKT_LEN, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr,
                 &len);  // Receiving packets successfully indicates that the IP
                         // and UDP packet formats are correct.

    // Read the check file
    sprintf(name, "%s%d%s", TEST_FILE_PREFIX, i, TEST_FILE_SUFFIX);
    FILE *fp;
    int fileLen;
    fp = fopen(name, "rb");
    fileLen = fileSize(name);
    fread(example, 1, fileLen, fp);
    fclose(fp);

    // Check length correction
    assert(n == fileLen);

    // Check payload correction
    for (int j = 0; j < fileLen; j++) {
      assert(example[j] == buffer[j]);
    }

    printf("Received %d bytes, test%d passed\n", n, i);
  }

  return 0;
}