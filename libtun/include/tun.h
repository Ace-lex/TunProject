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

int UDPTunSend(int tun, const char *src_ip, const char *dest_ip, int src_port,
               int dest_port, const u_int8_t *message, int payload_length);
int FileSize(const char *filename);
int TunCreate(char *dev, int flags);
socklen_t SockPrepare(int *sock_fd, struct sockaddr_in *server_addr,
                      struct sockaddr_in *client_addr, int port);
#endif