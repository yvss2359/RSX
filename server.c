

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
    

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sockad ={
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

 
   
    int rec = recv(sockfd, (void *)recievedMsg, MAXLINE, 0);
    recievedMsg[rec] = '\0';
    printf("Client : %s\n", recievedMsg);


    close(sockfd);
    
       
    return 0;
}


