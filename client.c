
#include <stdio.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>

#define PORT     8080
#define MAXLINE  1024
int main(){
    int sockfd;
    char recievedMsg[MAXLINE];
    char *sendMsg = "Hello from client";

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

   struct sockaddr_in sockad ={
       .sin_family    = AF_INET, 
       .sin_addr.s_addr =inet_addr("127.0.0.1"),
       .sin_port = 0,
       .sin_zero = 0
    };
    struct sockaddr_in sockaserv ={
       .sin_family    = AF_INET, 
       .sin_addr.s_addr =inet_addr("127.0.0.1"),
       .sin_port = PORT,
       .sin_zero = 0
    };
    

    
    if ( bind(sockfd, (const struct sockaddr *)&sockad, sizeof(sockad)) <0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
   

    int send = sendto(sockfd, (void*)sendMsg, strlen(sendMsg), 0, (const struct sockaddr *) &sockaserv,sizeof(sockaserv));
    assert(send != -1);
    printf("%s sent.\n", sendMsg); 
  
    // int shut = shutdown(sockfd,2);
    // assert(shut == -1);
     close(sockfd);

    return 0;
}


