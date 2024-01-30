#include <stdlib.h>
#include <stdio.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <string.h>
#include <unistd.h>

#define MAXLINE 4096 /* max text line length */
#define SERV_PORT 3000 /* port */
#define LISTENQ 8 /* maximum number of client connections */

double convertToIndianRupees(const char *currency, double value) {
    // Perform currency conversion logic here
    // For simplicity, let's assume a conversion factor for each currency
    if (strcmp(currency, "USD") == 0) {
        return value * 74.50; // Assuming 1 USD = 74.50 INR
    } else if (strcmp(currency, "EUR") == 0) {
        return value * 88.20; // Assuming 1 EUR = 88.20 INR
    } else if (strcmp(currency, "GBP") == 0) {
        return value * 102.75; // Assuming 1 GBP = 102.75 INR
    }
    return -1; // Invalid currency
}

int main(int argc, char **argv) {
    int listenfd, connfd, n;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    // Creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // Preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        // Loop to handle client requests
      for (int i = 0; i < 3; ++i) {
            if ((n = recv(connfd, buf, MAXLINE, 0)) <= 0) {
                break; // End of input
            }

            buf[n] = '\0'; // Ensure null-terminated string
            char currency[4];
            double value;

            // Parse currency and value from client's input
            if (sscanf(buf, "%s %lf", currency, &value) == 2) {
                double convertedValue = convertToIndianRupees(currency, value);
                snprintf(buf, MAXLINE, "%.2f INR", convertedValue);
            } else {
                snprintf(buf, MAXLINE, "Invalid input format");
            }

            // Send the response to the client
            send(connfd, buf, strlen(buf), 0);
}

        close(connfd);
    }

    // Close listening socket
    close(listenfd);

    exit(0);
}

