#include <linux/if_tun.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 	
#include <assert.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/udp.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstdlib>

//IP地址转为16进制表示
void convertIpToByteArray(const char *ipAddress,unsigned char *byteArray) 
{
    struct in_addr addr;

    // 将IP地址字符串转换为网络地址结构
    if (inet_pton(AF_INET, ipAddress, &addr) <= 0) {
        fprintf(stderr, "Invalid IP address\n");
        return;
    }

    // 将每个字节存储到数组中
    byteArray[0] = (unsigned char)addr.s_addr;
    byteArray[1] = (unsigned char)(addr.s_addr >> 8);
    byteArray[2] = (unsigned char)(addr.s_addr >> 16);
    byteArray[3] = (unsigned char)(addr.s_addr >> 24);

}

int fileSize(char* filename) 
{ 
  struct stat statbuf; 
  stat(filename,&statbuf); 
  int size=statbuf.st_size; 
 
  return size; 
}

int tunCreate(char *dev, int flags)
{
     struct ifreq ifr;
     int fd, err;

     assert(dev != NULL);

     if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
         return fd;

     memset(&ifr, 0, sizeof(ifr));
     ifr.ifr_flags |= flags;
     if (*dev != '\0')
         strncpy(ifr.ifr_name, dev, IFNAMSIZ);
     if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) 
     {
         close(fd);
         return err;
     }
     strcpy(dev, ifr.ifr_name);

    return fd;
}

// 计算UDP校验和
uint16_t calculateUDPChecksum(uint8_t* udpPacket, size_t udpLength) 
{
    // 如果UDP数据包长度为奇数，则添加一个零字节
    if (udpLength % 2 != 0) 
    {
        udpLength++;
        udpPacket[12+udpLength]='\0';
    }

    uint32_t sum = 0;

    // 将16位字节对相加
    for (size_t i = 0; i < udpLength; i += 2) 
    {
        uint16_t word = (udpPacket[i] << 8) + udpPacket[i + 1];
        sum += word;
    }

    // 将进位加到低16位
    while (sum >> 16) 
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // 取反
    uint16_t checksum = (uint16_t)(~sum);

    return checksum;
}

//计算IP头部校验和
uint16_t calculateIPChecksum(const uint8_t* ipHeader, size_t headerLength) 
{
    uint32_t sum = 0;

    // 将16位字节对相加
    for (size_t i = 0; i < headerLength; i += 2) 
    {
        uint16_t word = (ipHeader[i] << 8) + ipHeader[i + 1];
        sum += word;
    }

    // 将进位加到低16位
    while (sum >> 16) 
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // 取反
    uint16_t checksum = (uint16_t)(~sum);

    return checksum;
}

void printUDP(unsigned char *udpPacket,int udpLen)
{
    int i;
    for(i=0;i<udpLen;i++)
    {
        printf("0x%02x ",udpPacket[i]);
        fflush(stdout);
    }
    printf("\n");
}

int main( int argc, char * argv[ ] ) 
{ 
    int tun, ret; 
    char tunName[ IFNAMSIZ] ; 
    unsigned char buf[ 4096] ;
 
    tunName[ 0] = '\0' ; 
    tun = tunCreate( tunName, IFF_TUN | IFF_NO_PI) ; 
    if ( tun < 0) 
    { 
        perror ( "tunCreate" ) ; 
        return 1; 
    } 
    printf ( "TUN name is %s\n" , tunName) ; 
    fflush(stdout);
    system("./script.sh");
    
    for(int i=0;i<3;i++) 
    { 
        unsigned char sip[ 4] ;
        unsigned char dip[ 4] ;
        unsigned char port[2];
        unsigned short udpLen;
        unsigned short udpCheckSum;
        unsigned short ipCheckSum;
        unsigned char udpPacket[4096];
        char name[100];
        uint8_t protocal;
        char message[4096];
        int payloadLen;
        memset(message,0,sizeof(message));

        sprintf(name,"%s%d","test",i);
        payloadLen=fileSize(name);
        FILE *fp;
        fp=fopen(name,"rb");
        fread(message,1,payloadLen,fp);
        fclose(fp);

        *(unsigned short*)&buf[0]=0x0045;
        *(unsigned short*)&buf[4]=0xaabb;
        *(unsigned short*)&buf[6]=0x0040;
        buf[8]=64;
        buf[9]=17;
        convertIpToByteArray("10.10.10.1",sip);
        convertIpToByteArray("192.168.0.187",dip);
        memcpy(&buf[12],sip,4);
        memcpy(&buf[16],dip,4); 
        //UDP头部      
        *(unsigned short*)&buf[20]=38709;
        std::swap(buf[20],buf[21]);
        *(unsigned short*)&buf[22]=8080;
        std::swap(buf[22],buf[23]);
        udpLen=8+payloadLen;
        *((unsigned short*)&buf[24])=udpLen;
        std::swap(buf[24],buf[25]);
        memcpy(&buf[28],message,payloadLen);
        
        //计算校验和
        *((unsigned short*)&buf[26])=0x0000;
        memcpy(udpPacket,buf+12,4);
        memcpy(udpPacket+4,buf+16,4);
        *(unsigned short*)&udpPacket[8]=0x1100;
        memcpy(udpPacket+10,buf+24,2);
        memcpy(udpPacket+12,buf+20,udpLen);

        udpCheckSum=calculateUDPChecksum(udpPacket,udpLen+12);
        *((unsigned short*)&buf[26])=udpCheckSum;
        std::swap(buf[26],buf[27]);
        //printUDP(buf+20,udpLen);

        *(unsigned short*)&buf[2]=20+udpLen;
        std::swap(buf[2],buf[3]);
        *(unsigned short*)&buf[10]=0x0000;
        ipCheckSum=calculateIPChecksum(buf,20);
        *(unsigned short*)&buf[10]=ipCheckSum;
        std::swap(buf[10],buf[11]);
        ret = write ( tun, buf, 20+udpLen) ; 
        printf ( "write %d bytes\n" , ret) ; 
        fflush(stdout);
        sleep(1);
    } 
 
    return 0; 
} 