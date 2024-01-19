// Tun.cpp: functions used in sending udp packets by tun

#include "Tun.h"

// Create a tun device, returning the file description of the device
int tunCreate(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;

  assert(dev != NULL);

  if ((fd = open(TUN_FILE_NAME, O_RDWR)) < 0) return fd;

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
int fileSize(const char *filename) {
  struct stat statbuf;
  stat(filename, &statbuf);
  int size = statbuf.st_size;

  return size;
}

// Calculate the ip/udp checksum
uint16_t calculateChecksum(uint8_t *packet, size_t length) {
  //  If the length of the UDP datagram is odd, add a zero byte.
  if (length % 2 != 0) {
    packet[length] = '\0';
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
int udpTunSend(int tun, const char *hostSip, const char *hostDip, int sport,
               int dport, unsigned char *buf, const unsigned char *message,
               int payloadLen) {
  u_int32_t sip, dip;
  unsigned short udpLen;
  unsigned short udpCheckSum;
  unsigned short ipCheckSum;
  unsigned char udpPacket[PKT_LEN];
  char name[FILE_NAME_LEN];
  uint8_t protocal;
  struct iphdr *ipd = (struct iphdr *)(buf);
  struct udphdr *udpd = (struct udphdr *)(buf + sizeof(struct iphdr));
  unsigned short totLen;
  struct pseudo_hdr *psed = (struct pseudo_hdr *)(udpPacket);
  int ret;

  // IP header
  ipd->ihl = IHL;
  ipd->version = IPV;
  ipd->tos = TOS;
  ipd->id = htons(IPID);
  ipd->frag_off = htons(FGOF);
  ipd->ttl = TTL;
  ipd->protocol = IPPROTO_UDP;
  sip = inet_addr(hostSip);
  dip = inet_addr(hostDip);
  ipd->saddr = sip;
  ipd->daddr = dip;

  // UDP header
  udpd->source = htons(sport);
  udpd->dest = htons(dport);
  udpLen = UDPH_LEN + payloadLen;
  udpd->len = htons(udpLen);
  udpd->check = 0x0000;
  memcpy(&buf[IPH_LEN + UDPH_LEN], message, payloadLen);

  // Preparation for calculate the checksum
  psed->src = sip;
  psed->dst = dip;
  psed->mbz = 0;
  psed->protocol = IPPROTO_UDP;
  psed->len = udpd->len;
  memcpy(udpPacket + PSE_UDPH_LEN, buf + IPH_LEN, udpLen);

  // Calculate the udp checksum
  udpCheckSum = calculateChecksum(udpPacket, udpLen + PSE_UDPH_LEN);
  udpd->check = htons(udpCheckSum);

  // Calculate the ip checksum
  totLen = IPH_LEN + udpLen;
  ipd->tot_len = htons(totLen);
  ipd->check = 0x0000;
  ipCheckSum = calculateChecksum(buf, IPH_LEN);
  ipd->check = htons(ipCheckSum);

  // Send udp packets
  ret = write(tun, buf, IPH_LEN + udpLen);
  return ret;
}

// Create socket for receive packets sended by tun devices
socklen_t sockPre(int &sockfd, struct sockaddr_in &servaddr,
                  struct sockaddr_in &cliaddr, int port) {
  // socket file description
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Populate server information.
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(port);

  // Bind the server information to the socket
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return (socklen_t)sizeof(cliaddr);
}