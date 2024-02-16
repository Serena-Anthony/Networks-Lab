#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <dirent.h>

#define SERV_TCP_PORT 3000
#define MAX 4096

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char send[MAX], recvline[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(SERV_TCP_PORT);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    //file transmission
    printf("\nEnter the source file name : \n");
    scanf("%s", send);
    write(sockfd, send, MAX);
    FILE * fPtr = fopen(send, "w");
    while ((n = read(sockfd, recvline, MAX)) != 0) {
        printf("%s", recvline);
        fprintf(fPtr, "%s", recvline);
    }
    fclose(fPtr);

    close(sockfd);
    return 0;
}

