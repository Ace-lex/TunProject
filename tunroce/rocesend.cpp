// RoceSend.cpp: Send basic RoCEv2 packet using libTun.so

#include "tun.h"

const char *const kSrcIP = "10.10.10.1";
const char *const kDstIP = "192.168.0.39";
const int kSrcPort = 31233;
const int kDstPort = 4791;
const int kPartitionKey = 0xffff;
const int kDstQPN = 0x012c3f;
const int kPacketNum = 0x123456;
const int kFlagMask = 0x00ffffff;
const int kPSNMask = 0x00ffffff;
const int kQPNMask = 0x00ffffff;

const int kQueueKey = 0x1ee7a330;
const int kSrcQP = 0x012c41;
const int kIBHeaderLen = 20;
const uint8_t kOpcode = 100;
const char *const kTestFilePrefix = "test";
const char *kScriptAddr = "../script.sh";
const int kInternal = 1;  // send interval(second)
const int kPacketLen = 4096;
const int kPseudoHeaderLen = 12;

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
  system(kScriptAddr);

  // Send RoCEv2 packets
  while (true) {
    uint8_t udp_packet[kPacketLen];
    const char *name = kTestFilePrefix;
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
      pad_cnt = (payload_len % 4) << 4;
      payload_len = payload_len + 4 - payload_len % 4;
    }

    // Infiniband BTH
    bth->opcode = kOpcode;
    bth->flags = kFlagMask | pad_cnt;
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