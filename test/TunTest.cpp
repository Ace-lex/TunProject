// TunTest.cpp: transmission test for libTun.so.

#include "Tun.h"
#define TEST_FILE 3
#define SPORT 31233
#define DPORT 8080
#define SIP "10.10.10.1"
#define SCRIPT_ADDR "../script.sh"
#define TEST_FILE_PREFIX "test"
#define INTERVAL 1  // send interval

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char buf[PKT_LEN];

  // Create tun devices
  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);

  // configure the tun device
  system(SCRIPT_ADDR);

  // Send udp packets by tun devices
  for (int i = 0; i < TEST_FILE; i++) {
    unsigned char message[PKT_LEN];
    int payloadLen;
    char name[FILE_NAME_LEN];
    memset(message, 0, sizeof(message));

    // Read payload files
    sprintf(name, "%s%d", TEST_FILE_PREFIX, i);
    payloadLen = fileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message, 1, payloadLen, fp);
    fclose(fp);

    // Send UDP packets
    ret = udpTunSend(tun, SIP, (const char *)argv[1], SPORT, DPORT, buf,
                     message, payloadLen);

    // Ensure successful reception at the receiving end.
    sleep(INTERVAL);
  }

  return 0;
}