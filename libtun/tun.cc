// This file defines functions used in sending udp packets by tun

#include "tun.h"

#include <assert.h>
#include <sys/uio.h>

const char *const kTunFileName = "/dev/net/tun";  // Tun device file path
const int kIPID = 0xaabb;           // Identification of IP packets
const int kFlagAndOffset = 0x0000;  // The IP flag
const uint8_t kTTL = 64;            // The time to live of IP packets
const int kPacketLen = 65536;        // The max length of the packet
const int8_t kIPVersion = 4;        // IPv4
const int8_t kTOS = 0;              // Type of Service(Routine)
const int8_t kPacketPart = 3;  // Divide packet into 3 parts: IP + UDP + payload

// Returns the tun file description of the device
int TunCreate(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;

  assert(dev != NULL);

  if ((fd = open(kTunFileName, O_RDWR)) < 0) return fd;

  // Set the device name
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

int FileSize(const char *filename) {
  struct stat stat_buf;
  stat(filename, &stat_buf);
  int size = stat_buf.st_size;

  return size;
}

// Returns the ip/udp checksum
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

void SetIOVector(const struct iphdr *iph, const struct udphdr *udph,
                 const uint8_t *payload, int payload_length) {}

// Send udp packets by tun
// Given source and destination IP address and port, returning the number of
// successfully transmitted bytes.
int UDPTunSend(int tun, const char *src_ip, const char *dest_ip, int src_port,
               int dest_port, const uint8_t *payload, int payload_length) {
  int udp_len;
  int udp_checksum;
  int ip_checksum;
  uint8_t buf[kPacketLen] = {0};
  uint8_t udp_packet[kPacketLen] = {0};
  uint8_t protocal;
  struct iphdr *ip_header = (struct iphdr *)(buf);
  struct udphdr *udp_header = (struct udphdr *)(buf + sizeof(*ip_header));
  int total_len;
  struct pseudo_hdr *pseudo_header = (struct pseudo_hdr *)(udp_packet);
  int ret;

  // Check the parameter
  assert(src_port >= 0 && dest_port >= 0);
  assert(payload_length >= 0);

  // IP header
  ip_header->ihl = sizeof(*ip_header) / 4;
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
  udp_len = sizeof(*udp_header) + payload_length;
  udp_header->len = htons(udp_len);
  udp_header->check = 0x0000;
  memcpy(&buf[sizeof(*ip_header) + sizeof(*udp_header)], payload,
         payload_length);

  // Preparation for calculate the checksum
  pseudo_header->src_ip = ip_header->saddr;
  pseudo_header->dst_ip = ip_header->daddr;
  pseudo_header->mbz = 0;
  pseudo_header->protocol = IPPROTO_UDP;
  pseudo_header->len = udp_header->len;
  memcpy(udp_packet + sizeof(*pseudo_header), buf + sizeof(*ip_header),
         udp_len);

  // Calculate the udp checksum
  udp_checksum =
      CalculateChecksum(udp_packet, udp_len + sizeof(*pseudo_header));
  udp_header->check = htons(udp_checksum);

  // Calculate the ip checksum
  total_len = sizeof(*ip_header) + udp_len;
  ip_header->tot_len = htons(total_len);
  ip_header->check = 0x0000;
  ip_checksum = CalculateChecksum(buf, sizeof(*ip_header));
  ip_header->check = htons(ip_checksum);

  // Send udp packets
  ret = write(tun, buf, sizeof(*ip_header) + udp_len);
  if (ret < 0) {
    fprintf(stderr, "Error sending packets: %s\n", strerror(errno));
  }
  return ret;
}

// The function are same as UDPTunSend, but use writev system call to encapture
// the packet
int UDPTunSendv2(int tun, const char *src_ip, const char *dest_ip, int src_port,
                 int dest_port, const uint8_t *payload, int payload_length) {
  int udp_len;
  int udp_checksum;
  int ip_checksum;
  uint8_t udp_packet[kPacketLen] = {0};
  uint8_t protocal;
  struct iphdr ip_header;
  struct udphdr udp_header;
  int total_len;
  struct pseudo_hdr *pseudo_header = (struct pseudo_hdr *)(udp_packet);
  struct iovec iov[kPacketPart];
  int ret;

  // Check the parameter
  assert(src_port >= 0 && dest_port >= 0);
  assert(payload_length >= 0);

  // IP header
  ip_header.ihl = sizeof(ip_header) / 4;
  ip_header.version = kIPVersion;
  ip_header.tos = kTOS;
  ip_header.id = htons(kIPID);
  ip_header.frag_off = htons(kFlagAndOffset);
  ip_header.ttl = kTTL;
  ip_header.protocol = IPPROTO_UDP;
  ip_header.saddr = inet_addr(src_ip);
  ip_header.daddr = inet_addr(dest_ip);

  // UDP header
  udp_header.source = htons(src_port);
  udp_header.dest = htons(dest_port);
  udp_len = sizeof(udp_header) + payload_length;
  udp_header.len = htons(udp_len);
  udp_header.check = 0x0000;

  // Preparation for calculate the checksum
  pseudo_header->src_ip = ip_header.saddr;
  pseudo_header->dst_ip = ip_header.daddr;
  pseudo_header->mbz = 0;
  pseudo_header->protocol = IPPROTO_UDP;
  pseudo_header->len = udp_header.len;
  memcpy(udp_packet + sizeof(*pseudo_header), &udp_header, sizeof(udp_header));
  memcpy(udp_packet + sizeof(*pseudo_header) + sizeof(udp_header), payload,
         payload_length);

  // Calculate the udp checksum
  udp_checksum =
      CalculateChecksum(udp_packet, udp_len + sizeof(*pseudo_header));
  udp_header.check = htons(udp_checksum);

  // Calculate the ip checksum
  total_len = sizeof(ip_header) + udp_len;
  ip_header.tot_len = htons(total_len);
  ip_header.check = 0x0000;
  ip_checksum =
      CalculateChecksum((const uint8_t *)&ip_header, sizeof(ip_header));
  ip_header.check = htons(ip_checksum);

  // Send udp packets
  iov[0].iov_base = &ip_header;
  iov[0].iov_len = sizeof(ip_header);
  iov[1].iov_base = &udp_header;
  iov[1].iov_len = sizeof(udp_header);
  iov[2].iov_base = (void *)payload;
  iov[2].iov_len = payload_length;
  ret = writev(tun, iov, kPacketPart);
  if (ret < 0) {
    fprintf(stderr, "Error sending packets: %s\n", strerror(errno));
  }
  return ret;
}

// Create socket for receive packets sended by tun devices, returning the length
// of sockaddr
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