// This file tests the UDPTunSend function

#include "tun.h"

const int kNumTestFile = 3;  // The number of test cases
const int kSrcPort = 31233;  // The source port of tested packets
const int kDestPort = 8080;  // The destination port of tested packets
const char *const kSrcIP =
    "10.10.10.1";  // The source ip of tested packets(refer the script)
const char *const kScriptPath =
    "../script.sh";  // The script path to configure tun device
const char *const kTestFilePrefix = "./testfile/test";  // The testfile path
const char *const kTestFileSuffix = ".bin";             // The testfile suffix
const int kInternal = 1;       // The send interval(second)
const int kFileNameLen = 100;  // The max length of filename
const int kPacketLen = 4096;   // The max length of packet length

void TestUDPTunSend(int tun, const char *dst_ip, bool is_writev) {
  int ret;
  for (int i = 0; i < kNumTestFile; i++) {
    uint8_t message[kPacketLen];
    int payload_length;
    char name[kFileNameLen];
    memset(message, 0, sizeof(message));

    // Read payload files
    sprintf(name, "%s%d%s", kTestFilePrefix, i, kTestFileSuffix);
    payload_length = FileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    if (fp == NULL) {
      fprintf(stderr, "Error opening the file: %s\n", strerror(errno));
    }
    fread(message, 1, payload_length, fp);
    fclose(fp);

    // Send UDP packets
    if (!is_writev) {
      ret = UDPTunSend(tun, kSrcIP, dst_ip, kSrcPort, kDestPort, message,
                       payload_length);
    } else {
      ret = UDPTunSendv2(tun, kSrcIP, dst_ip, kSrcPort, kDestPort, message,
                         payload_length);
    }

    // Ensure successful reception at the receiving end.
    sleep(kInternal);
  }
}

int main(int argc, char *argv[]) {
  int tun, ret;
  char tun_name[IFNAMSIZ];
  const char *dst_ip = argv[1];

  // Create tun devices
  tun_name[0] = '\0';
  tun = TunCreate(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }

  // Configure the tun device
  system(kScriptPath);

  // Test UDPTunSend(use memcpy)
  TestUDPTunSend(tun, dst_ip, false);

  // Test UDPTunSendv2(use writev)
  TestUDPTunSend(tun, dst_ip, true);

  return 0;
}