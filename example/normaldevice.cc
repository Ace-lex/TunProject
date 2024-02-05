// NormalDevice.cpp: communicate with the TUN device.
// Usage: ./NormalDevice [filename]

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

const int kRecvPort = 8080;   // The recving port of socket
const int kPacketLen = 4096;  // The max length of packet
const char *const kDestIP =
    "10.10.10.1";             // The destination IP of packets(tun device IP)

int FileSize(const char *filename) {
  struct stat stat_buf;
  stat(filename, &stat_buf);
  int size = stat_buf.st_size;

  return size;
}

int main(int argc, char *argv[]) {
  int sock_fd;
  char buffer[kPacketLen];
  struct sockaddr_in server_addr;
  int payload_len;

  // Create the socket file description
  if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  // printf("socket successfully built\n");
  memset(&server_addr, 0, sizeof(server_addr));

  // Populate the server information
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(kRecvPort);
  server_addr.sin_addr.s_addr = inet_addr(kDestIP);

  socklen_t len = (socklen_t)sizeof(server_addr);  // len is value/resuslt

  // Send message
  while (true) {
    // Read payload file or get payload from stdin
    if (argc == 1) {
      printf("input payload: ");
      fgets(buffer, kPacketLen, stdin);
      payload_len = strlen(buffer);
      buffer[strcspn(buffer, "\n")] = 0;
    } else {
      FILE *fp = fopen(argv[1], "rb");
      payload_len = FileSize(argv[1]);
      fread(buffer, 1, payload_len, fp);
      fclose(fp);
    }

    // Send packets to tun device
    sendto(sock_fd, (const char *)buffer, payload_len, 0,
           (const struct sockaddr *)&server_addr, len);
    fprintf(stdout, "message: %s have sent.\n", buffer);
    fflush(stdout);

    // Receive packets from tun device
    int n = recvfrom(sock_fd, (char *)buffer, kPacketLen, MSG_WAITALL,
                     (struct sockaddr *)&server_addr, &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
  }

  close(sock_fd);
  return 0;
}