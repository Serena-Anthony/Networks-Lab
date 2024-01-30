#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define MAXLINE 1024
#define LISTENQ 10

typedef struct sockaddr SA;

int main (int argc, char **argv){
    int sockfd, n;
    char recvline [MAXLINE + 1];
    struct sockaddr_in servaddr;
    int port=8000;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // Creates a TCP Socket
            printf("Socket Error");
    }


    bzero(&servaddr, sizeof(servaddr)); // fills servaddr with zeros.

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

 
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
     // Establishes a TCP Connnection
        printf("Connect Error.\n");
    }

    while ((n=read(sockfd, recvline, MAXLINE)) > 0){
    // reads server reply.
        recvline[n] =0;

        if (fputs(recvline, stdout) == EOF){
            printf("fputs error.\n");
        }
    }
    if (n <0){
        printf("read error");
    }
    
    }
