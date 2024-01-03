
#include "Tun.h"
#define PORT 8080

int main() {
  int sockfd;
  unsigned char buffer[PKT_LEN];
  char sendbuffer[PKT_LEN];
  unsigned char example[PKT_LEN];
  struct sockaddr_in servaddr, cliaddr;
  char name[100];
  int n;

  socklen_t len = sockPre(sockfd, servaddr, cliaddr, PORT);

  for (int i = 0; i < 3; i++) {
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

    printf("received %d bytes, success\n", n);
  }

  return 0;
}