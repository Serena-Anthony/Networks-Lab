#include<stdio.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#define SERV_TCP_PORT 3000
#define MAX 4096

void *handle_client(void *sock_desc) {
    int newsockfd = *(int*)sock_desc;
    char file_name[MAX];
    char buff[4096];

    read(newsockfd, &file_name, MAX);
    printf("\nClient message\n File Name : %s\n", file_name);
    
    FILE *f1 = fopen(file_name, "r");
    if (f1 == NULL) {
        printf("File not found\n");
        close(newsockfd);
        pthread_exit(NULL);
    }

    while (fgets(buff, 4096, f1) != NULL) {
    	printf("%s", buff);
        write(newsockfd, buff, MAX);
    }

    fclose(f1);
    printf("\nFile Transferred\n");
    close(newsockfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, clength;
    struct sockaddr_in serv_addr, cli_addr;

    //socket creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    //socket intitialization
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERV_TCP_PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    printf("\nBinded");
    listen(sockfd, 5);
    printf("\nListening...");

    //accept connections in loop
    while (1) {
        clength = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clength);

        //create thread to handle client 
        pthread_t helper;
        pthread_create(&helper, NULL, handle_client, (void*)&newsockfd);   
        pthread_detach(helper);
    }

    close(sockfd);
    return 0;
}

