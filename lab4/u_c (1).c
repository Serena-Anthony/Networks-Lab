#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"  // Replace with the server's IP address
#define SERVER_PORT 3000

int main(void) {
    int client_fd;
    struct sockaddr_in server_address;

    // Create a UDP socket
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Initialize the server's address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // User input for integer values
    int valuet, valueb, value_s;
    while (1) {
        printf("Enter a temperature value (or type 'stop' to quit): ");
        char temp[50];
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        // Check for exit condition
        if (strcmp(temp, "stop") == 0) {
            break;
        }

        printf("Enter a battery value:");
        char bat[50];
        fgets(bat, sizeof(bat), stdin);
        bat[strcspn(bat, "\n")] = '\0';
  
  
       printf("Enter client number:");
        char num[50];
        fgets(num, sizeof(num), stdin);
        num[strcspn(num, "\n")] = '\0';
        
        // Convert user input to an integer
        valuet = atoi(temp);
        // Send the integer value to the server
        sendto(client_fd, &valuet, sizeof(valuet), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        
        valueb = atoi(bat);
        // Send the integer value to the server
        sendto(client_fd, &valueb, sizeof(valueb), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        
        value_s = atoi(num);
sendto(client_fd, &value_s, sizeof(value_s), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    }

    // Signal closure to the server by sending -1
    valuet = -1;
    sendto(client_fd, &valuet, sizeof(valuet), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    
    valueb = -1;
    sendto(client_fd, &valueb, sizeof(valueb), 0, (struct sockaddr*)&server_address, sizeof(server_address));

    value_s = -1;
    sendto(client_fd, &value_s, sizeof(value_s), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    // Close the socket
    close(client_fd);

    return 0;
}

