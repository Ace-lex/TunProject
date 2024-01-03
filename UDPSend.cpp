
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

  // 创建socket文件描述符
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  // printf("socket successfully built\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // 填充服务端信息
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr("10.10.10.1");

  socklen_t len = (socklen_t)sizeof(servaddr);  // len is value/resuslt

  // 发送消息
  while (1) {
    if (argc == 1) {
      fgets(buffer, MAXLINE, stdin);
      payloadLen = strlen(buffer);
    } else {
      FILE *fp = fopen(argv[1], "rb");
      payloadLen = fileSize(argv[1]);
      fread(buffer, 1, payloadLen, fp);
      fclose(fp);
    }
    sendto(sockfd, (const char *)buffer, payloadLen, 0,
           (const struct sockaddr *)&servaddr, len);
    fprintf(stdout, "message: %s have sent.\n", buffer);
    fflush(stdout);
    int n =
        recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                 (struct sockaddr *)&servaddr, &len);  //收到信息便说明格式正确
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
    // assert(!strcmp(buffer,"reply by tu"));
  }

  close(sockfd);
  return 0;
}