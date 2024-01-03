#include "Tun.h"
#define DPORT 8080

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char buf[PKT_LEN];

  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tun_create");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);
  while (1) {
    struct in_addr sip;
    struct iphdr *recvIph = (struct iphdr *)(buf);
    struct udphdr *recvUdph = (struct udphdr *)(buf + 20);
    unsigned char udpPacket[PKT_LEN];
    uint8_t protocal;
    unsigned char message[4096];
    int payloadLen;
    memset(message, 0, sizeof(message));
    if (argc < 2) {
      sprintf((char *)message, "reply by tun");
      payloadLen = strlen((char *)message);
    } else {
      payloadLen = fileSize(argv[1]);
      FILE *fp;
      fp = fopen(argv[1], "rb");
      fread(message, 1, payloadLen, fp);
      fclose(fp);
    }
    ret = read(tun, buf, sizeof(buf));
    if (ret < 0) break;
    protocal = recvIph->protocol;
    printf("read %d bytes", ret);
    fflush(stdout);
    memcpy(&sip, &recvIph->saddr, 4);
    printf("from %s\n", inet_ntoa(sip));
    fflush(stdout);
    if (protocal == 17) {
      printf("Received udp packet, source port is %d, payload size is %d, ",
             ntohs(recvUdph->source), ntohs(recvUdph->len));
      ret = udpTunSend(tun, ntohs(recvUdph->source), buf, message, payloadLen);
      printf("write %d bytes\n", ret);
      fflush(stdout);
    }
  }

  return 0;
}