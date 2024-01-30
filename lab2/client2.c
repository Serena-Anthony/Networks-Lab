#include <stdlib.h>
#include <stdio.h>
//#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096 /* max text line length */
#define SERV_PORT 3000 /* port */

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    // Basic check of the arguments
    // Additional checks can be inserted
    if (argc != 2) {
        perror("Usage: TCPClient <IP address of the server");
        exit(1);
    }

    // Create a socket for the client
    // If sockfd < 0 there was an error in the creation of the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT); // convert to big-endian order

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    }

    // Input loop for providing three currency-value pairs
    for (int i = 0; i < 3; ++i) {
        printf("Enter currency and value (e.g., USD 100): ");
        if (fgets(sendline, MAXLINE, stdin) == NULL) {
            break; // End of input
        }

        // Send user input to the server
        send(sockfd, sendline, strlen(sendline), 0);

        // Receive and display the server's response
        if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
            // Error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }
        printf("Equivalent value in Indian currency: %s\n", recvline);
    }

    // Close the socket
    close(sockfd);

    exit(0);
}

