// Receive the packets sended by tun and check the correctness

#include <dirent.h>
#include <sys/time.h>

#include "tun.h"

const int kNumTestFile = 3;                  // The number of test cases
const int kRecvPort = 8080;                  // The receiving port of socket
const int kPacketLen = 4096;                 // The max length of packet length
const int kFileNameLen = 100;                // The max length of filename
const char *const kTestFileDir =
    "./testfile";                            // The script path to configure tun device
const char *const kTestFileSuffix = ".bin";  // The testfile suffix
const int kWaitTime = 10;                    // The timeout of test

void RecvAndCheck(const char *dir_name, int sock_fd,
                  struct sockaddr_in *client_addr, socklen_t len) {
  uint8_t example[kPacketLen];
  uint8_t buffer[kPacketLen];
  int n;

  DIR *dir;
  struct dirent *entry;
  char path[kFileNameLen];

  // Open payload directory
  dir = opendir(dir_name);
  
  if (dir == NULL) {
    perror("Error opening directory");
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      // Receive the packets
      memset(example, 0, sizeof(example));
      n = recvfrom(sock_fd, buffer, kPacketLen, MSG_WAITALL,
                   (struct sockaddr *)client_addr,
                   &len);  // Receiving packets successfully indicates that the
                           // IP and UDP packet formats are correct.

      // Read the check file
      sprintf(path, "%s/%s", dir_name, entry->d_name);
      FILE *fp = fopen(path, "rb");
      int file_length;
      file_length = FileSize(path);
      fread(example, 1, file_length, fp);
      fclose(fp);

      // Check length correction
      assert(n == file_length);

      // Check payload correction
      for (int j = 0; j < file_length; j++) {
        assert(example[j] == buffer[j]);
      }

      printf("Received %d bytes, test passed\n", n);
    }
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
  RecvAndCheck(kTestFileDir, sock_fd, &client_addr, len);

  printf("====UDPTunSend(use writev)====\n");
  fflush(stdout);
  RecvAndCheck(kTestFileDir, sock_fd, &client_addr, len);

  return 0;
}