// RoceSend.cpp: Send basic RoCEv2 packet using libTun.so

#include "tun.h"

#define SIP "10.10.10.1"
#define DIP "192.168.0.39"
#define SPORT 31233
#define DPORT 4791
#define BTH_DEF_PKEY (0xffff)
#define DQPN 0x012c3f
#define PSN 0x123456
#define BTH_FLAG_MASK 0x40
#define BTH_PSN_MASK (0x00ffffff)
#define BTH_QPN_MASK (0x00ffffff)
#define QKEY 0x1ee7a330
#define SQP 0x012c41
#define IBH_LEN 20
#define OPCODE 100  // UD SEND
#define TEST_FILE_PREFIX "test"
#define SCRIPT_ADDR "../script.sh"

const int kInternal = 1;  // send interval(second)
const int kFileNameLen = 100;
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
  char tunName[IFNAMSIZ];

  // Create tun device
  tunName[0] = '\0';
  tun = TunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);

  // configure the tun device
  system(SCRIPT_ADDR);

  // Send RoCEv2 packets
  while (true) {
    uint8_t udpPacket[kPacketLen];
    const char name[kFileNameLen] = TEST_FILE_PREFIX;
    uint8_t protocal;
    uint8_t message[kPacketLen];
    struct rxe_bth *bth = (struct rxe_bth *)message;
    struct rxe_deth *deth = (struct rxe_deth *)&message[kPseudoHeaderLen];
    int payloadLen;
    uint8_t setPadCnt;
    memset(message, 0, sizeof(message));

    // Read payload files
    payloadLen = FileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message + IBH_LEN, 1, payloadLen, fp);
    fclose(fp);
    if (payloadLen % 4 != 0) {
      setPadCnt = (payloadLen % 4) << 4;
      payloadLen = payloadLen + 4 - payloadLen % 4;
    }

    // Infiniband BTH
    bth->opcode = OPCODE;
    bth->flags = BTH_FLAG_MASK | setPadCnt;
    bth->pkey = htons(BTH_DEF_PKEY);
    bth->qpn = htonl(DQPN & BTH_QPN_MASK);
    bth->apsn = htonl(PSN & BTH_PSN_MASK);

    // DETH
    deth->qkey = htonl(QKEY);
    deth->sqp = htonl(SQP & BTH_QPN_MASK);

    // Send packet
    ret = UDPTunSend(tun, SIP, DIP, SPORT, DPORT, message,
                     payloadLen + sizeof(rxe_bth) + sizeof(rxe_deth));

    printf("write %d bytes\n", ret);
    fflush(stdout);
    //  Ensure successful reception at the receiving end.
    sleep(kInternal);
  }

  return 0;
}