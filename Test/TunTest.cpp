//用于进行传输测试，需通过命令行传入目的地址（本机IP）

#include "../libTun/Tun.h"
#define TEST_FILE 3
#define SPORT 31233
#define DPORT 8080
#define SIP "10.10.10.1"
#define DIP "192.168.0.39"

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char buf[PKT_LEN];

  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);
  system("../script.sh");
  // sleep(5);
  for (int i = 0; i < TEST_FILE; i++) {
    unsigned char message[PKT_LEN];
    int payloadLen;
    char name[FILE_NAME_LEN];
    memset(message, 0, sizeof(message));

    sprintf(name, "%s%d", "test", i);
    payloadLen = fileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message, 1, payloadLen, fp);
    fclose(fp);

    ret = udpTunSend(tun, SIP, (const char *)argv[1], SPORT, DPORT, buf,
                     message, payloadLen);

    sleep(1);
  }

  return 0;
}