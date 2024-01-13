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
#define FILE_NAME_LEN 100
#define IPID 0xaabb
#define FGOF 0x4000
#define TTL 64
#define PKT_LEN 4096
#define IPH_LEN 20
#define UDPH_LEN 8
#define PSE_UDPH_LEN 12
#define IPV 4
#define IHL 5
#define TOS 0

struct pseudo_hdr {
  u_int32_t src;  //源IP地址，32bit；看源程序中ip_src_addr和ip_dst_addr类型而定

  u_int32_t dst;  //目的IP地址，32bit

  char mbz;  //全0，8bit

  char protocol;  //协议字段，8bit

  short int len;  // UDP长度，16bit；UDP首部+净荷总长
};

int udpTunSend(int tun, const char *hostSip, const char *hostDip, int sport,
               int dport, unsigned char *buf, unsigned char *message,
               int payloadLen);
int fileSize(char *filename);
int tunCreate(char *dev, int flags);
socklen_t sockPre(int &sockfd, struct sockaddr_in &servaddr,
                  struct sockaddr_in &cliaddr, int port);
#endif
