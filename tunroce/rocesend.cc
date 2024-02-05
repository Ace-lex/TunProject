// This file sends basic RoCEv2 packets using libTun.so

#include "tun.h"

const char *const kSrcIP = "10.10.10.1";    // Source IP of packets
const char *const kDstIP = "192.168.0.39";  // Destination of packets(host)
const int kSrcPort = 31233;                 // Source port of packets
const int kDstPort = 4791;                  // Destination port of packets(RoCEv2 is 4791)
const int kPartitionKey = 0xffff;           // Partition key of RoCEv2 packets
const int kDstQPN = 0x012c3f;               // Destination queue pair number
const int kPacketNum = 0x123456;            // Packet sequence number
const int kPSNMask = 0x00ffffff;            // PSN has only 24bits
const int kQPNMask = 0x00ffffff;            // QPN has only 24bits

const int kQueueKey = 0x1ee7a330;
const int kSrcQP = 0x012c41;  // Source QP
const int kIBHeaderLen = 20;  // Infiniband header length
const uint8_t kOpcode = 100;  // opcode(UD Send)
const char *const kPayloadFilePath =
    "./payload/test.bin";  // Path of the payload file
const char *kScriptPath =
    "../script.sh";               // The script path to configure tun device
const int kInternal = 1;          // Send interval(second)
const int kPacketLen = 4096;      // The max length of the packet
const int kPseudoHeaderLen = 12;  // The length of udp pseudo header

// the struct of BTH
struct rxe_bth {
  u_int8_t opcode;
  u_int8_t flags;
  u_int16_t pkey;
  u_int32_t qpn;
  u_int32_t apsn;
};

// the struct of DETH
struct rxe_deth {
  __be32 qkey;
  __be32 sqp;
};

int main(int argc, char *argv[]) {
  int tun, ret;
  char tun_name[IFNAMSIZ];

  // Create tun device
  tun_name[0] = '\0';
  tun = TunCreate(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tun_name);
  fflush(stdout);

  // configure the tun device
  system(kScriptPath);

  // Send RoCEv2 packets
  while (true) {
    uint8_t udp_packet[kPacketLen];
    const char *name = kPayloadFilePath;
    uint8_t protocal;
    uint8_t message[kPacketLen];
    struct rxe_bth *bth = (struct rxe_bth *)message;
    struct rxe_deth *deth = (struct rxe_deth *)&message[kPseudoHeaderLen];
    int payload_len;
    uint8_t pad_cnt;
    memset(message, 0, sizeof(message));

    // Read payload files
    payload_len = FileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message + kIBHeaderLen, 1, payload_len, fp);
    fclose(fp);
    if (payload_len % 4 != 0) {
      pad_cnt = 4 - payload_len % 4;
      payload_len = payload_len + 4 - payload_len % 4;
    }

    // Infiniband BTH
    bth->opcode = kOpcode;
    bth->flags = pad_cnt << 4;
    bth->pkey = htons(kPartitionKey);
    bth->qpn = htonl(kDstQPN & kQPNMask);
    bth->apsn = htonl(kPacketNum & kPSNMask);

    // DETH
    deth->qkey = htonl(kQueueKey);
    deth->sqp = htonl(kSrcQP & kQPNMask);

    // Send packet
    ret = UDPTunSend(tun, kSrcIP, kDstIP, kSrcPort, kDstPort, message,
                     payload_len + sizeof(rxe_bth) + sizeof(rxe_deth));

    printf("write %d bytes\n", ret);
    fflush(stdout);

    //  Ensure successful reception at the receiving end.
    sleep(kInternal);
  }

  return 0;
}