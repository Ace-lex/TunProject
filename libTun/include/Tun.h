// Tun.h: head file of the dynamic library
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

#define TUN_FILE_NAME "/dev/net/tun"
#define FILE_NAME_LEN 100
#define IPID 0xaabb
#define FGOF 0x0000
#define TTL 64
#define PKT_LEN 4096
#define IPH_LEN 20
#define UDPH_LEN 8
#define PSE_UDPH_LEN 12
#define IPV 4
#define IHL 5
#define TOS 0

struct pseudo_hdr {
  u_int32_t src;  // source ip address

  u_int32_t dst;  // destination ip address

  char mbz;  // zero

  char protocol;  // protocol field

  short int len;  // length of udp packet
};

int udpTunSend(int tun, const char *hostSip, const char *hostDip, int sport,
               int dport, unsigned char *buf, const unsigned char *message,
               int payloadLen);
int fileSize(const char *filename);
int tunCreate(char *dev, int flags);
socklen_t sockPre(int &sockfd, struct sockaddr_in &servaddr,
                  struct sockaddr_in &cliaddr, int port);
#endif
