// Tun.cpp: functions used in sending udp packets by tun

#include "tun.h"

#include <assert.h>

const char *const kTunFileName = "/dev/net/tun";
const int kFileNameLen = 100;
const int kIPID = 0xaabb;
const int kFlagAndOffset = 0x4000;
const uint8_t kTTL = 64;
const int kPacketLen = 4096;
const int kIPHeaderLen = 20;
const int kUDPHeaderLen = 8;
const int kPseudoHeaderLen = 12;
const int8_t kIPVersion = 4;
const int8_t kTOS = 0;

// Create a tun device, returning the file description of the device
int TunCreate(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;

  assert(dev != NULL);

  if ((fd = open(kTunFileName, O_RDWR)) < 0) return fd;

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

// Return the file size
int FileSize(const char *filename) {
  struct stat stat_buf;
  stat(filename, &stat_buf);
  int size = stat_buf.st_size;

  return size;
}

// Calculate the ip/udp checksum
uint16_t CalculateChecksum(const uint8_t *packet, size_t length) {
  //  If the length of the UDP datagram is odd, add a zero byte.
  if (length % 2 != 0) {
    length++;
  }

  uint32_t sum = 0;

  // Add the 16-bit byte pairs.
  for (size_t i = 0; i < length; i += 2) {
    uint16_t word = (packet[i] << 8) + packet[i + 1];
    sum += word;
  }

  // Add the carry to the low 16 bits.
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  // Negate
  uint16_t checksum = (uint16_t)(~sum);

  return checksum;
}

// Send udp packets by tun
// Take in source and destination IP address and port, returning the number of
// successfully transmitted bytes.

// About buf and message: 'message' is used to convey the payload
// There are other ways to design the API, such as retaining only 'buf' param,
// which can save memory space but may be inconvenient to users.
// The function can also internally create a buffer 'buf'
// and only pass the 'message' parameter.
int UDPTunSend(int tun, const char *src_ip, const char *dest_ip, int src_port,
               int dest_port, const u_int8_t *message, int payload_length) {
  int udp_len;
  int udp_checksum;
  int ip_checksum;
  uint8_t buf[kPacketLen] = {0};
  uint8_t udp_packet[kPacketLen] = {0};
  char name[kFileNameLen];
  uint8_t protocal;
  struct iphdr *ip_header = (struct iphdr *)(buf);
  struct udphdr *udp_header = (struct udphdr *)(buf + sizeof(struct iphdr));
  int total_len;
  struct pseudo_hdr *pseudo_header = (struct pseudo_hdr *)(udp_packet);
  int ret;

  // Check the parameter
  assert(src_port >= 0 && dest_port >= 0);
  assert(payload_length >= 0);

  // IP header
  ip_header->ihl = kIPHeaderLen / 4;
  ip_header->version = kIPVersion;
  ip_header->tos = kTOS;
  ip_header->id = htons(kIPID);
  ip_header->frag_off = htons(kFlagAndOffset);
  ip_header->ttl = kTTL;
  ip_header->protocol = IPPROTO_UDP;
  ip_header->saddr = inet_addr(src_ip);
  ip_header->daddr = inet_addr(dest_ip);

  // UDP header
  udp_header->source = htons(src_port);
  udp_header->dest = htons(dest_port);
  udp_len = kUDPHeaderLen + payload_length;
  udp_header->len = htons(udp_len);
  udp_header->check = 0x0000;
  memcpy(&buf[kIPHeaderLen + kUDPHeaderLen], message, payload_length);

  // Preparation for calculate the checksum
  pseudo_header->src = ip_header->saddr;
  pseudo_header->dst = ip_header->daddr;
  pseudo_header->mbz = 0;
  pseudo_header->protocol = IPPROTO_UDP;
  pseudo_header->len = udp_header->len;
  memcpy(udp_packet + kPseudoHeaderLen, buf + kIPHeaderLen, udp_len);

  // Calculate the udp checksum
  udp_checksum = CalculateChecksum(udp_packet, udp_len + kPseudoHeaderLen);
  udp_header->check = htons(udp_checksum);

  // Calculate the ip checksum
  total_len = kIPHeaderLen + udp_len;
  ip_header->tot_len = htons(total_len);
  ip_header->check = 0x0000;
  ip_checksum = CalculateChecksum(buf, kIPHeaderLen);
  ip_header->check = htons(ip_checksum);

  // Send udp packets
  ret = write(tun, buf, kIPHeaderLen + udp_len);
  return ret;
}

// Create socket for receive packets sended by tun devices
socklen_t SockPrepare(int *sock_fd, struct sockaddr_in *server_addr,
                      struct sockaddr_in *client_addr, int port) {
  // socket file description
  if (((*sock_fd) = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(server_addr, 0, sizeof(struct sockaddr_in));
  memset(client_addr, 0, sizeof(struct sockaddr_in));

  // Populate server information.
  server_addr->sin_family = AF_INET;  // IPv4
  server_addr->sin_addr.s_addr = INADDR_ANY;
  server_addr->sin_port = htons(port);

  // Bind the server information to the socket
  if (bind(*sock_fd, (const struct sockaddr *)server_addr,
           sizeof(sockaddr_in)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return (socklen_t)sizeof(sockaddr_in);
}