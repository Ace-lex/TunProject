// Client side implementation of UDP client-server model
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
 
#define PORT 8080
#define MAXLINE 1024
  
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in     servaddr;
  
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    //printf("socket successfully built\n");
    memset(&servaddr, 0, sizeof(servaddr));
      
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("10.10.10.1");
      
    socklen_t len = (socklen_t)sizeof(servaddr);  //len is value/resuslt
  
    // send msg
    while (fgets(buffer, MAXLINE, stdin) != NULL) {
        int i = strlen(buffer);
        if (buffer[i - 1] == '\n') {
            buffer[i - 1] = 0;
        }
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, 
                (const struct sockaddr *) &servaddr, len);
        fprintf(stdout, "message: %s have sent.\n", buffer);
        fflush(stdout);
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, 
                    (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
        printf("Server : %s\n", buffer);   
    }

    close(sockfd);
    return 0;
}