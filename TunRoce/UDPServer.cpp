// Server side implementation of UDP client-server model
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <assert.h>
  
#define PORT    4791
#define MAXLINE 4096

int file_size(char* filename) 
{ 
  struct stat statbuf; 
  stat(filename,&statbuf); 
  int size=statbuf.st_size; 
 
  return size; 
}

// Driver code
int main() {
    int sockfd;
    unsigned char buffer[MAXLINE];
    char sendbuffer[MAXLINE];
    unsigned char example[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    char name[100];
      
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
      
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
      
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
      
    int n;
    socklen_t len = (socklen_t)sizeof(cliaddr);  //len is value/resuslt
  
 
 
    for (;;) {
        memset(example,0,sizeof(example));
        n = recvfrom(sockfd, buffer, MAXLINE, 
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                    &len);//接收到说明IP包格式和UDP格式正确
        printf("received %d bytes, success\n",n);
    }
 
    return 0;
}