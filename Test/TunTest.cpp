#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/if.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

// IP地址转为16进制表示
void convertIpToByteArray(const char *ipAddress, unsigned char *byteArray) {
  struct in_addr addr;

  // 将IP地址字符串转换为网络地址结构
  if (inet_pton(AF_INET, ipAddress, &addr) <= 0) {
    fprintf(stderr, "Invalid IP address\n");
    return;
  }

  // 将每个字节存储到数组中
  byteArray[0] = (unsigned char)addr.s_addr;
  byteArray[1] = (unsigned char)(addr.s_addr >> 8);
  byteArray[2] = (unsigned char)(addr.s_addr >> 16);
  byteArray[3] = (unsigned char)(addr.s_addr >> 24);
}

int fileSize(char *filename) {
  struct stat statbuf;
  stat(filename, &statbuf);
  int size = statbuf.st_size;

  return size;
}

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

// 计算UDP校验和
uint16_t calculateChecksum(uint8_t *packet, size_t length) {
  // 如果UDP数据包长度为奇数，则添加一个零字节
  if (length % 2 != 0) {
    length++;
    packet[12 + length] = '\0';
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

void printUDP(unsigned char *udpPacket, int udpLen) {
  int i;
  for (i = 0; i < udpLen; i++) {
    printf("0x%02x ", udpPacket[i]);
    fflush(stdout);
  }
  printf("\n");
}

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
    u_int32_t sip, dip;
    // unsigned char sip[4];
    // unsigned char dip[4];
    unsigned char port[2];
    unsigned short udpLen;
    unsigned short udpCheckSum;
    unsigned short ipCheckSum;
    unsigned char udpPacket[4096];
    char name[100];
    uint8_t protocal;
    char message[4096];
    int payloadLen;
    struct iphdr *ipd = (struct iphdr *)(buf);
    struct udphdr *udpd = (struct udphdr *)(buf + sizeof(struct iphdr));
    unsigned short totLen;
    memset(message, 0, sizeof(message));

    sprintf(name, "%s%d", "test", i);
    payloadLen = fileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message, 1, payloadLen, fp);
    fclose(fp);

    ipd->ihl = 5;
    ipd->version = 4;
    ipd->tos = 0;
    ipd->id = htons(0xaabb);
    ipd->frag_off = htons(0x4000);
    ipd->ttl = 64;
    ipd->protocol = 17;
    sip = inet_addr("10.10.10.1");
    dip = inet_addr("192.168.0.39");
    ipd->saddr = sip;
    ipd->daddr = dip;

    // UDP头部
    udpd->source = htons(38709);
    udpd->dest = htons(8080);
    udpLen = 8 + payloadLen;
    udpd->len = htons(udpLen);
    memcpy(&buf[28], message, payloadLen);

    //计算校验和
    *((unsigned short *)&buf[26]) = 0x0000;
    memcpy(udpPacket, buf + 12, 4);
    memcpy(udpPacket + 4, buf + 16, 4);
    *(unsigned short *)&udpPacket[8] = 0x1100;
    memcpy(udpPacket + 10, buf + 24, 2);
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
    printf("write %d bytes\n", ret);
    fflush(stdout);
    sleep(1);
  }

  return 0;
}