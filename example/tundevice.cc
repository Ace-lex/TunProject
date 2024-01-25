// Example of sending and recving udp packets by tun device
// Usage: ./TunDevice [filename]

#include "tun.h"

const char *const kDefaultMessage =
    "reply by tun";  // The default payload of reply packets(if payload file is
                     // not provided)
const int kIPAddrLen = 15;  // Length of IP address in dotted decimal notation"
const char *kScriptPath =
    "../script.sh";           // The script path to configure tun device
const int kIPHeaderLen = 20;  // IP header length(usually)
const int kPacketLen = 4096;  // The max length of packet

int main(int argc, char *argv[]) {
  int tun, ret;
  char tun_name[IFNAMSIZ];
  uint8_t recv_buf[kPacketLen];

  // Create tun device
  tun_name[0] = '\0';
  tun = TunCreate(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tun_create");
    return 1;
  }
  printf("TUN name is %s\n", tun_name);
  fflush(stdout);

  // configure the tun device
  system(kScriptPath);

  // Receive and send UDP packet
  while (true) {
    struct in_addr src_ip;
    struct in_addr dst_ip;
    char reply_src_ip[kIPAddrLen];
    char reply_dst_ip[kIPAddrLen];
    struct iphdr *recv_IP_header = (struct iphdr *)(recv_buf);
    struct udphdr *recv_UDP_header = (struct udphdr *)(recv_buf + kIPHeaderLen);
    uint8_t protocal;
    uint8_t message[kPacketLen];
    int payload_len;

    // Read payload file or reply default message
    memset(message, 0, sizeof(message));
    if (argc < 2) {
      sprintf((char *)message, kDefaultMessage);
      payload_len = strlen((char *)message);
    } else {
      payload_len = FileSize(argv[1]);
      FILE *fp;
      fp = fopen(argv[1], "rb");
      fread(message, 1, payload_len, fp);
      fclose(fp);
    }

    // Receive packet using tun device
    ret = read(tun, recv_buf, sizeof(recv_buf));
    if (ret < 0) break;
    protocal = recv_IP_header->protocol;
    printf("read %d bytes ", ret);
    fflush(stdout);
    memcpy(&src_ip, &recv_IP_header->saddr, sizeof(in_addr));
    strcpy(reply_dst_ip, (const char *)inet_ntoa(src_ip));
    printf("from %s\n", reply_dst_ip);
    fflush(stdout);
    memcpy(&dst_ip, &recv_IP_header->daddr, sizeof(in_addr));
    strcpy(reply_src_ip, (const char *)inet_ntoa(dst_ip));

    // Check whether the received data packet is a UDP packet.
    if (protocal == IPPROTO_UDP) {
      printf("Received udp packet, source port is %d, payload size is %d, ",
             ntohs(recv_UDP_header->source), ntohs(recv_UDP_header->len));

      // Reply UDP packet
      ret = UDPTunSend(tun, reply_src_ip, reply_dst_ip,
                       ntohs(recv_UDP_header->dest),
                       ntohs(recv_UDP_header->source), message, payload_len);

      printf("write %d bytes\n", ret);
      fflush(stdout);
    }
  }

  return 0;
}