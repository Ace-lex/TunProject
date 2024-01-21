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

struct pseudo_hdr {
  u_int32_t src;  // source ip address

  u_int32_t dst;  // destination ip address

  char mbz;  // zero

  char protocol;  // protocol field

  short int len;  // length of udp packet
};

int udpTunSend(int tun, const char *hostSip, const char *hostDip, int sport,
               int dport, const u_int8_t *message, int payloadLen);
int fileSize(const char *filename);
int tunCreate(char *dev, int flags);
socklen_t sockPre(int *sockfd, struct sockaddr_in *servaddr,
                  struct sockaddr_in *cliaddr, int port);
#endif
