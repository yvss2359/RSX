#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#define QR_HEADER_LEN 12
#define QR_ADDIT_LEN 5
#define DESTPORT "53"
#define NS_ANSWER_MAXLEN 512 
#define DESTIPV4 "127.0.0.53"


void fill_header(unsigned char* query){
    unsigned char base[QR_HEADER_LEN] = {
        0x08,  0xbb,  0x01,  0x00,
        0x00,  0x01,  0x00,  0x00,
        0x00,  0x00,  0x00,  0x00,
    };    
    for (size_t i = 0; i < QR_HEADER_LEN; i++){
        query[i] = base[i];
    }
}

void fill_footer(unsigned char* query, size_t query_size){
    query[query_size-5] = 0x00;
    query[query_size-4] = 0x00;
    query[query_size-3] = 0x01;
    query[query_size-2] = 0x00;
    query[query_size-1] = 0x01;
}


int subdomain_size(char const *adrs_url, int index){

    int i_domain = 1;
    unsigned char const *p =(unsigned char const*) adrs_url;

    assert(index > 0 );
    assert(index <= 3);
    
    while (i_domain < index) {
        if (p[0] == '.') {
            i_domain++;
        }
        p++;
    }

    assert(i_domain < 4);
    //printf("%s\n",p);

    int res = 0;
    while ((p[0] != '.') && *p){
        p++;
        res++;
    }  
    return res;
}


void build_qname(char const* adrs_url, unsigned char* query){

    int i;
    int i_domain = 1;
    unsigned char const *p = adrs_url;

    query[QR_HEADER_LEN] = subdomain_size(adrs_url, i_domain);

    for (i = QR_HEADER_LEN+1; *p; i++){
        if ('.' != *p){
            query[i] = *p;
        }
        else {
            i_domain++;
            query[i] = subdomain_size(adrs_url, i_domain);
        }
        p++;
    }
    query[i] = 0x00; // zero terminateur
}

void build_request(char const *adrs_url, unsigned char* query, int query_size){

    fill_header(query);
    build_qname(adrs_url, query);
    fill_footer(query, query_size);
    printf("Request building ... [###]\n");
    for (int i=0; i<query_size;i++){
         printf("%.2X", query[i] & 0xff);
    }
    printf("... [ok]\n");
}


void display_answer(int rcv_size, unsigned char * answer){
     for (int i = 0; i < rcv_size; i++) {

        printf(" %.2X", answer[i] & 0xff);

        if (((i+1)%16 == 0) || (i+1 == rcv_size)) {
            for (int j = i+1 ; j < ((i+16) & ~15); j++) {
            fprintf(stdout,"   ");
            }
            printf("\t");
            for (int j = i & ~15; j <= i; j++)
            printf("%c",answer[j] > 31 && answer[j] < 128 ? (char)answer[j] : '.');
            printf("\n");
        }
    }
    printf("Answer reception from server ...[ok]\n");


}



void decode_ip_add(unsigned char * answer){
    unsigned char * c  = answer+12;
    printf("   -@IP : ");
    for(int i = 0; i<4; i++){
        printf("%d", (int)c[i]);
        if(i<3) {
            printf(".");
        }
    }
    printf("\n");
}



void decode_answer(unsigned char*answer,int len,int urlen){
      int type;
      int rd_data_len;
      int answer_size;
      int answer_nb = *(answer+7);
      int cur_answer = 1;
      unsigned char *isPointer = answer+urlen;

    //printf("pointer : %s\n", isPointer);
    assert(*isPointer == 0xc0);
    printf(" +number of responses: %d \n", answer_nb);

    while(cur_answer<=answer_nb){
/*        while(*isPointer=!0xc0){
            isPointer++;
        }*/

        
        printf("  -response: %d \n", cur_answer);

        type = (int)isPointer[3];
        rd_data_len = isPointer[11];
        answer_size = 12+ rd_data_len;
         //printf(" type: %d \n", type);
        if(type==1){
            printf("  -->type : ip address\n");
            //afficher l'adresse ip
            decode_ip_add(isPointer);
        }
        else if(type==5){
            printf("  -->type : cannonic name\n");
            // afficher l'adresse cannonique
        }

        isPointer = isPointer + answer_size; 

        cur_answer++;
    }

	
}


int main(int argc, char const *argv[]){


    assert(argc == 2);

    // building request
    char const *hostname = argv[1];
    int hostname_size = strlen(hostname) + 1;
    int query_size = QR_HEADER_LEN + hostname_size + QR_ADDIT_LEN;
    unsigned char* query = (unsigned char*) malloc(query_size*sizeof(unsigned char));

    build_request(hostname, query, query_size);

    //Obtention of remote @IP address

    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET; //IPv4 
    hints.ai_socktype = SOCK_DGRAM;//UDP 

    struct addrinfo *res = NULL;
    int addrfd=getaddrinfo(DESTIPV4,DESTPORT,&hints,&res);
    printf("Obtention of remote @IP address ...[###]\n");
    if(addrfd!=0){
        printf("error: %s\n", gai_strerror(addrfd));
        exit(EXIT_FAILURE);
    }
    printf("Obtention of remote @IP address ...[ok]\n");


    // Creation and Sending a socket request to server

    printf("Sending request to server ...[###] \n");
    int sock;
    sock=socket(res->ai_family, res->ai_socktype,res->ai_protocol);
    if(sock==-1){
        perror("socket creation error\n");
        exit(EXIT_FAILURE);
    }
    int send=sendto(sock,
        query,query_size,
        0,
        (const struct sockaddr *)res->ai_addr,sizeof(struct sockaddr_in));

    if(send==-1){
        perror("sendto error \n");
        exit(EXIT_FAILURE);
    }
    printf("%d bytes sended\n", send);

    printf("Sending request to server ...[ok] \n");


    //Answer reception from server

    printf("Answer reception from server ...[###]\n");


    unsigned char answer[NS_ANSWER_MAXLEN];

    struct sockaddr_in addrRemoteFromRecv;
    socklen_t addrRemoteFromRecvlen = sizeof(struct sockaddr_in);
    int rcv = recvfrom(sock, answer, NS_ANSWER_MAXLEN, 0, (struct sockaddr *)&addrRemoteFromRecv,&addrRemoteFromRecvlen);

    
    printf("%d bytes recieved\n",rcv);
    if(rcv==-1){
        perror("recvfrom error\n");
        close(sock);
        exit(EXIT_FAILURE);
    }
    close(sock);
    printf(" - port  distant  : %hu\n", ntohs(addrRemoteFromRecv.sin_port));
    printf(" - @IPv4 distante : %s\n", inet_ntoa(addrRemoteFromRecv.sin_addr));
    
    //Displaying the response recieved from the server
    display_answer(rcv,answer);
    
    //Deconding the answer recieved from the server
    printf("Decode the DNS reponse ... [###]\n");
    
    decode_answer(answer,rcv,query_size);
    
    printf("Decode the DNS reponse ... [ok]\n");
    
   

    free(query);


/*

    int   nb;
 
    nb = 0;
    nb = rand() % 100;
    printf("\n\n%d\n", nb);
  */
    return 0;

	}





