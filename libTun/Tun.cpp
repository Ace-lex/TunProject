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

int udpTunSend(int tun, int dport, unsigned char *buf, unsigned char *message,
               int payloadLen) {
  u_int32_t sip, dip;
  unsigned short udpLen;
  unsigned short udpCheckSum;
  unsigned short ipCheckSum;
  unsigned char udpPacket[PKT_LEN];
  char name[100];
  uint8_t protocal;
  struct iphdr *ipd = (struct iphdr *)(buf);
  struct udphdr *udpd = (struct udphdr *)(buf + sizeof(struct iphdr));
  unsigned short totLen;
  struct pseudo_hdr *psed = (struct pseudo_hdr *)(udpPacket);
  int ret;

  ipd->ihl = 5;
  ipd->version = 4;
  ipd->tos = 0;
  ipd->id = htons(IPID);
  ipd->frag_off = htons(FGOF);
  ipd->ttl = TTL;
  ipd->protocol = 17;
  sip = inet_addr(SRCIP);
  dip = inet_addr(DSTIP);
  ipd->saddr = sip;
  ipd->daddr = dip;

  // UDP头部
  udpd->source = htons(SPORT);
  udpd->dest = htons(dport);
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
  memcpy(udpPacket + 12, buf + IPHDLEN, udpLen);

  udpCheckSum = calculateChecksum(udpPacket, udpLen + 12);
  udpd->check = htons(udpCheckSum);

  totLen = IPHDLEN + udpLen;
  ipd->tot_len = htons(totLen);
  ipd->check = 0x0000;
  ipCheckSum = calculateChecksum(buf, IPHDLEN);
  ipd->check = htons(ipCheckSum);
  ret = write(tun, buf, IPHDLEN + udpLen);
  return ret;
}

socklen_t sockPre(int &sockfd, struct sockaddr_in &servaddr,
                  struct sockaddr_in &cliaddr, int port) {
  // socket文件描述符
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // 填充服务端信息
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  // 绑定
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return (socklen_t)sizeof(cliaddr);
}