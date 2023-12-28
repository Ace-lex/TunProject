#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/if_tun.h>
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

uint16_t calculateChecksum(uint8_t *udpPacket, size_t udpLength) {
  // 如果UDP数据包长度为奇数，则添加一个零字节
  if (udpLength % 2 != 0) {
    udpLength++;
    udpPacket[12 + udpLength] = '\0';
  }

  uint32_t sum = 0;

  // 将16位字节对相加
  for (size_t i = 0; i < udpLength; i += 2) {
    uint16_t word = (udpPacket[i] << 8) + udpPacket[i + 1];
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

void printPayload(unsigned char *udpPayload, int payloadLen) {
  int i;
  printf("payload is ");
  for (i = 0; i < payloadLen; i++) {
    printf("%c", udpPayload[i]);
    fflush(stdout);
  }
  printf("\n");
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
    perror("tun_create");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);
  while (1) {
    unsigned char ip[4];
    unsigned char port[2];
    unsigned short udpLen;
    unsigned short udpCheckSum;
    unsigned short ipCheckSum;
    unsigned short recvUDPLen;
    unsigned char udpPacket[4096];
    uint8_t protocal;
    char message[4096];
    int payloadLen;
    memset(message, 0, sizeof(message));
    if (argc < 2) {
      sprintf(message, "reply by tun");
      payloadLen = strlen(message);
    } else {
      payloadLen = fileSize(argv[1]);
      FILE *fp;
      fp = fopen(argv[1], "rb");
      fread(message, 1, payloadLen, fp);
      fclose(fp);
    }
    ret = read(tun, buf, sizeof(buf));
    if (ret < 0) break;
    protocal = buf[9];
    memcpy(ip, &buf[12], 4);
    printf("read %d bytes", ret);
    fflush(stdout);
    printf("from %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    fflush(stdout);
    // printf("%d\n",protocal);
    if (protocal == 17) {
      printf("Received udp packet, source port is %d, payload size is %d, ",
             *(unsigned short *)&buf[20], buf[24] * 256 + buf[25] - 8);
      // printPayload(buf+28,buf[24]*256+buf[25]-8);
      convertIpToByteArray("10.10.10.1", ip);
      memcpy(&buf[16], &buf[12], 4);
      memcpy(&buf[12], ip, 4);
      *(unsigned short *)&buf[4] = 0xaabb;

      // UDP头部
      memcpy(port, &buf[20], 2);
      memcpy(&buf[20], &buf[22], 2);
      memcpy(&buf[22], port, 2);
      udpLen = 8 + payloadLen;
      *((unsigned short *)&buf[24]) = udpLen;
      std::swap(buf[24], buf[25]);
      memcpy(&buf[28], message, payloadLen);

      //计算校验和
      *((unsigned short *)&buf[26]) = 0x0000;
      memcpy(udpPacket, buf + 12, 4);
      memcpy(udpPacket + 4, buf + 16, 4);
      *(unsigned short *)&udpPacket[8] = 0x1100;
      memcpy(udpPacket + 10, buf + 24, 2);
      memcpy(udpPacket + 12, buf + 20, udpLen);
      // printf("before checksum, udpLen=%d\n",udpLen);
      // fflush(stdout);
      udpCheckSum = calculateChecksum(udpPacket, udpLen + 12);
      *((unsigned short *)&buf[26]) = udpCheckSum;
      std::swap(buf[26], buf[27]);
      // printUDP(buf+20,udpLen);

      *(unsigned short *)&buf[2] = 20 + udpLen;
      std::swap(buf[2], buf[3]);
      *(unsigned short *)&buf[10] = 0x0000;
      ipCheckSum = calculateChecksum(buf, 20);
      *(unsigned short *)&buf[10] = ipCheckSum;
      std::swap(buf[10], buf[11]);
      ret = write(tun, buf, 20 + udpLen);
      printf("write %d bytes\n", ret);
      fflush(stdout);
    }
  }

  return 0;
}