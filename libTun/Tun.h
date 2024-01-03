#ifndef __TUN_H__
#define __TUN_H__

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
#define IPID 0xaabb
#define FGOF 0x4000
#define SPORT 38709
#define TTL 64
#define PKT_LEN 4096
#define SRCIP "10.10.10.1"
#define DSTIP "192.168.0.39"
#define IPHDLEN 20

struct pseudo_hdr {
  u_int32_t src;  //源IP地址，32bit；看源程序中ip_src_addr和ip_dst_addr类型而定
  // struct in_addr dst;
  u_int32_t dst;  //目的IP地址，32bit
  // uint8_t mbz;
  char mbz;  //全0，8bit
  // uint8_t protocol;
  char protocol;  //协议字段，8bit
  // uint16_t len;
  short int len;  // UDP长度，16bit；UDP首部+净荷总长
};

int udpTunSend(int tun, int dport, unsigned char *buf, unsigned char *message,
               int payloadLen);
int fileSize(char *filename);
int tunCreate(char *dev, int flags);
socklen_t sockPre(int &sockfd, struct sockaddr_in &servaddr,
                  struct sockaddr_in &cliaddr, int port);
#endif
