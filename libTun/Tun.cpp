#include "Tun.h"

int tunCreate(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;

  assert(dev != NULL);

  if ((fd = open("/dev/net/tun", O_RDWR)) < 0) return fd;

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags |= flags;
  if (*dev != '\0') strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
    close(fd);
    return err;
  }
  strcpy(dev, ifr.ifr_name);

  return fd;
}

int fileSize(char *filename) {
  struct stat statbuf;
  stat(filename, &statbuf);
  int size = statbuf.st_size;

  return size;
}

uint16_t calculateChecksum(uint8_t *packet, size_t length) {
  // 如果UDP数据包长度为奇数，则添加一个零字节
  if (length % 2 != 0) {
    packet[length] = '\0';
    length++;
  }

  uint32_t sum = 0;

  // 将16位字节对相加
  for (size_t i = 0; i < length; i += 2) {
    uint16_t word = (packet[i] << 8) + packet[i + 1];
    sum += word;
  }

  // 将进位加到低16位
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  // 取反
  uint16_t checksum = (uint16_t)(~sum);

  return checksum;
}

int udpTunSend(int tun, unsigned char *buf, unsigned char *message,
               int payloadLen) {
  u_int32_t sip, dip;
  unsigned short udpLen;
  unsigned short udpCheckSum;
  unsigned short ipCheckSum;
  unsigned char udpPacket[4096];
  char name[100];
  uint8_t protocal;
  struct iphdr *ipd = (struct iphdr *)(buf);
  struct udphdr *udpd = (struct udphdr *)(buf + sizeof(struct iphdr));
  unsigned short totLen;
  struct pseudo_hdr *psed = (struct pseudo_hdr *)(udpPacket);
  int ret;
  // memset(message, 0, sizeof(message));

  // sprintf(name, "%s%d", "test", i);
  // payloadLen = fileSize(name);
  // FILE *fp;
  // fp = fopen(name, "rb");
  // fread(message, 1, payloadLen, fp);
  // fclose(fp);

  ipd->ihl = 5;
  ipd->version = 4;
  ipd->tos = 0;
  ipd->id = htons(IPID);
  ipd->frag_off = htons(FGOF);
  ipd->ttl = TTL;
  ipd->protocol = 17;
  sip = inet_addr("10.10.10.1");
  dip = inet_addr("192.168.0.39");
  ipd->saddr = sip;
  ipd->daddr = dip;

  // UDP头部
  udpd->source = htons(SPORT);
  udpd->dest = htons(DPORT);
  udpLen = 8 + payloadLen;
  udpd->len = htons(udpLen);
  udpd->check = 0x0000;
  memcpy(&buf[28], message, payloadLen);

  //计算校验和
  psed->src = sip;
  psed->dst = dip;
  psed->mbz = 0;
  psed->protocol = 17;
  psed->len = udpd->len;
  memcpy(udpPacket + 12, buf + 20, udpLen);

  udpCheckSum = calculateChecksum(udpPacket, udpLen + 12);
  udpd->check = htons(udpCheckSum);

  totLen = 20 + udpLen;
  ipd->tot_len = htons(totLen);
  ipd->check = 0x0000;
  *(unsigned short *)&buf[10] = 0x0000;
  ipCheckSum = calculateChecksum(buf, 20);
  ipd->check = htons(ipCheckSum);
  ret = write(tun, buf, 20 + udpLen);
  return ret;
}