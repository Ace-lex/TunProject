// TunDevice.cpp: example of sending and recving udp packets by tun device
// Usage: ./TunDevice [filename]

#include "Tun.h"

#define DEFAULT_MESSAGE "reply by tun"
#define IP_ADDR_LEN 15
#define SCRIPT_ADDR "../script.sh"

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char recvBuf[PKT_LEN];

  // Create tun device
  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tun_create");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);

  // configure the tun device
  system(SCRIPT_ADDR);

  // Receive and send UDP packet
  while (true) {
    struct in_addr sip;
    struct in_addr dip;
    char hostSip[IP_ADDR_LEN];
    char hostDip[IP_ADDR_LEN];
    struct iphdr *recvIph = (struct iphdr *)(recvBuf);
    struct udphdr *recvUdph = (struct udphdr *)(recvBuf + IPH_LEN);
    unsigned char udpPacket[PKT_LEN];
    uint8_t protocal;
    unsigned char message[PKT_LEN];
    int payloadLen;

    // Read payload file or reply default message
    memset(message, 0, sizeof(message));
    if (argc < 2) {
      sprintf((char *)message, DEFAULT_MESSAGE);
      payloadLen = strlen((char *)message);
    } else {
      payloadLen = fileSize(argv[1]);
      FILE *fp;
      fp = fopen(argv[1], "rb");
      fread(message, 1, payloadLen, fp);
      fclose(fp);
    }

    // Receive packet using tun device
    ret = read(tun, recvBuf, sizeof(recvBuf));
    if (ret < 0) break;
    protocal = recvIph->protocol;
    printf("read %d bytes ", ret);
    fflush(stdout);
    memcpy(&sip, &recvIph->saddr, sizeof(in_addr));
    strcpy(hostDip, (const char *)inet_ntoa(sip));
    printf("from %s\n", hostDip);
    fflush(stdout);
    memcpy(&dip, &recvIph->daddr, sizeof(in_addr));
    strcpy(hostSip, (const char *)inet_ntoa(dip));

    // Check whether the received data packet is a UDP packet.
    if (protocal == IPPROTO_UDP) {
      printf("Received udp packet, source port is %d, payload size is %d, ",
             ntohs(recvUdph->source), ntohs(recvUdph->len));

      // Reply UDP packet
      ret = udpTunSend(tun, hostSip, hostDip, ntohs(recvUdph->dest),
                       ntohs(recvUdph->source), message, payloadLen);

      printf("write %d bytes\n", ret);
      fflush(stdout);
    }
  }

  return 0;
}