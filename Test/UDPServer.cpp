
#include "Tun.h"
#define TEST_FILE 3
#define PORT 8080

int main() {
  int sockfd;
  unsigned char buffer[PKT_LEN];
  char sendbuffer[PKT_LEN];
  unsigned char example[PKT_LEN];
  struct sockaddr_in servaddr, cliaddr;
  char name[FILE_NAME_LEN];
  int n;

  socklen_t len = sockPre(sockfd, servaddr, cliaddr, PORT);

  for (int i = 0; i < TEST_FILE; i++) {
    memset(example, 0, sizeof(example));
    n = recvfrom(sockfd, buffer, PKT_LEN, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr,
                 &len);  //接收到说明IP包格式和UDP格式正确
    sprintf(name, "%s%d", "test", i);
    FILE *fp;
    int fileLen;
    fp = fopen(name, "rb");
    fileLen = fileSize(name);
    fread(example, 1, fileLen, fp);
    fclose(fp);
    assert(n == fileLen);
    for (int j = 0; j < fileLen; j++) {
      assert(example[j] == buffer[j]);
    }

    printf("Received %d bytes, test%d passed\n", n, i);
  }

  return 0;
}