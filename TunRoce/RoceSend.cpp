
#include "Tun.h"
#define DPORT 4791
#define BTH_DEF_PKEY (0xffff)
#define DQPN 0x012c3f
#define PSN 0x123456
#define BTH_PSN_MASK (0x00ffffff)
#define BTH_QPN_MASK (0x00ffffff)
#define QKEY 0x1ee7a330
#define SQP 0x012c41

struct rxe_bth {
  u_int8_t opcode;
  u_int8_t flags;
  u_int16_t pkey;
  u_int32_t qpn;
  u_int32_t apsn;
};

struct rxe_deth {
  __be32 qkey;
  __be32 sqp;
};

int main(int argc, char *argv[]) {
  int tun, ret;
  char tunName[IFNAMSIZ];
  unsigned char buf[4096];

  tunName[0] = '\0';
  tun = tunCreate(tunName, IFF_TUN | IFF_NO_PI);
  if (tun < 0) {
    perror("tunCreate");
    return 1;
  }
  printf("TUN name is %s\n", tunName);
  fflush(stdout);
  system("./script.sh");

  for (;;) {
    memset(buf, 0, sizeof(buf));
    unsigned char udpPacket[4096];
    char name[100];
    uint8_t protocal;
    unsigned char message[PKT_LEN];
    struct rxe_bth *bth = (struct rxe_bth *)message;
    struct rxe_deth *deth = (struct rxe_deth *)&message[12];
    int payloadLen;
    unsigned char setPdCnt;
    memset(message, 0, sizeof(message));

    sprintf(name, "%s", "test");
    payloadLen = fileSize(name);
    FILE *fp;
    fp = fopen(name, "rb");
    fread(message + 20, 1, payloadLen, fp);
    fclose(fp);
    if (payloadLen % 4 != 0) {
      setPdCnt = (payloadLen % 4) << 4;
      payloadLen = payloadLen + 4 - payloadLen % 4;
    }

    // Infiniband BTH
    bth->opcode = 100;
    bth->flags = 0x40 | setPdCnt;
    bth->pkey = htons(BTH_DEF_PKEY);
    bth->qpn = htonl(DQPN & BTH_QPN_MASK);
    bth->apsn = htonl(PSN & BTH_PSN_MASK);

    // DETH
    deth->qkey = htonl(QKEY);
    deth->sqp = htonl(SQP & BTH_QPN_MASK);
    ret = udpTunSend(tun, DPORT, buf, message,
                     payloadLen + sizeof(rxe_bth) + sizeof(rxe_deth));

    printf("write %d bytes\n", ret);
    fflush(stdout);
    sleep(1);
  }

  return 0;
}