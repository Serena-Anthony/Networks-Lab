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

int main(int argc, char **argv){
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buff [MAXLINE];
    time_t ticks;
    
    int port;
    socklen_t len;
    listenfd = socket(AF_INET, SOCK_STREAM, 0); // Creates a TCP Socket

    //Initialize the server address by the port and IP
    port = 8000;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    
//bind function to establish a local address for the socket
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    printf("Server is waiting connection at port %d\t\n", port);
  // listen allows servers to prepare a socket for incoming connections
    listen(listenfd, LISTENQ);

    for ( ; ; ) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &len);
        //Once a socket has been established, the server needs to wait for a connection. To do so, the server calls function accept. A call to accept blocks until a new connection request arrives.
        
        printf("Connection from %s, port %d\n", inet_ntop(AF_INET,
        &cliaddr.sin_addr.s_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));
        close(connfd);
    }
    
}
