#include "Tun.h"
#define DPORT 8080

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char buf[4096];

  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);
  system("./script.sh");
  // sleep(5);
  for (int i = 0; i < 3; i++) {
    unsigned char message[PKT_LEN];
    int payloadLen;
    char name[100];
    memset(message, 0, sizeof(message));

    sprintf(name, "%s%d", "test", i);
    payloadLen = fileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message, 1, payloadLen, fp);
    fclose(fp);

    ret = udpTunSend(tun, DPORT, buf, message, payloadLen);

    printf("write %d bytes\n", ret);
    fflush(stdout);
    sleep(1);
  }

  return 0;
}