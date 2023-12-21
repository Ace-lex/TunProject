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

int tun_create(char *dev, int flags)
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
     if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
         close(fd);
         return err;
     }
     strcpy(dev, ifr.ifr_name);

     return fd;
}

 
int main( int argc, char * argv[ ] ) 
{ 
        int tun, ret; 
        char tun_name[ IFNAMSIZ] ; 
        unsigned char buf[ 4096] ; 
 
         tun_name[ 0] = '\0' ; 
         tun = tun_create( tun_name, IFF_TUN | IFF_NO_PI) ; 
        if ( tun < 0) { 
                perror ( "tun_create" ) ; 
                return 1; 
        } 
        printf ( "TUN name is %s\n" , tun_name) ; 
        fflush(stdout);
 
        while ( 1) { 
                unsigned char ip[ 4] ; 
 
                 ret = read ( tun, buf, sizeof ( buf) ) ; 
                if ( ret < 0) 
                        break ; 
                memcpy ( ip, & buf[ 12] , 4) ; 
                // memcpy ( & buf[ 12] , & buf[ 16] , 4) ; 
                // memcpy ( & buf[ 16] , ip, 4) ; 
                //  buf[ 20] = 0; 
                // * ( ( unsigned short * ) & buf[ 22] )  = * ( ( unsigned short * ) & buf[ 22] )+8; 
                printf ( "read %d bytes\n" , ret) ; 
                fflush(stdout);
                printf("from %d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
                fflush(stdout);
                // ret = write ( tun, buf, ret) ; 
                // printf ( "write %d bytes\n" , ret) ; 
                // fflush(stdout);
        } 
 
        return 0; 
} 