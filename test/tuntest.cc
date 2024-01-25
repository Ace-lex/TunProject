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

int main(int argc, char *argv[]) {
  int tun, ret;
  char tun_name[IFNAMSIZ];

  // Create tun devices
  tun_name[0] = '\0';
  tun = TunCreate(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tun_name);
  fflush(stdout);

  // Configure the tun device
  system(kScriptPath);

  // Send udp packets by tun devices
  for (int i = 0; i < kNumTestFile; i++) {
    unsigned char message[kPacketLen];
    int payload_length;
    char name[kFileNameLen];
    memset(message, 0, sizeof(message));

    // Read payload files
    sprintf(name, "%s%d%s", kTestFilePrefix, i, kTestFileSuffix);
    payload_length = FileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message, 1, payload_length, fp);
    fclose(fp);

    // Send UDP packets
    ret = UDPTunSend(tun, kSrcIP, (const char *)argv[1], kSrcPort, kDestPort,
                     message, payload_length);

    // Ensure successful reception at the receiving end.
    sleep(kInternal);
  }

  return 0;
}