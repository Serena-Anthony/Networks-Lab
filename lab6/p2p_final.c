#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define LISTENQ 8 // maximum number of pending connections in the listen queue

struct thread_arguments // Define a structure for thread arguments
{
    char ip[15]; // IP address
    int port;    // Port number
};

static void *sender(void *arg)
{
    struct thread_arguments * args;
    args = (struct thread_arguments*)arg;

    struct sockaddr_in addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd <0)
    {

        perror("Problem in creating the socket");
        exit(2);
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family =AF_INET;
    addr.sin_port = htons(args->port);
    addr.sin_addr.s_addr = inet_pton(sockfd, args->ip, &addr);

    sleep(2);
  if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) // Connect to the server
    {
        perror("Problem in connecting to the server");
        exit(3);
    }
    else
    {
        printf("Connection success\n");
    }
     char buffer[BUFFER_SIZE]; //  a buffer for storing the message

    while (fgets(buffer, BUFFER_SIZE,stdin) !=NULL)
    {
        send(sockfd, buffer, strlen(buffer) + 1, 0); // Send the message to the server
    }
}

//reciever->server
static void* reciever (void * arg)
{
     struct thread_arguments *args;
    args = (struct thread_arguments *)arg; // Cast the argument to the thread arguments structure

    struct sockaddr_in cliaddr, servaddr;

    socklen_t len = sizeof(struct sockaddr_in);


    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) // Create a TCP socket for listening
    {
        perror("Problem in creating the socket");
        exit(2);
    }

    memset(&servaddr, 0, sizeof(servaddr));    // Clear the server address structure
    servaddr.sin_family = AF_INET;                // Set the address family to IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Set the IP address to any available interface
    servaddr.sin_port = htons(args->port);        // Set the port number

    bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // Bind the socket to the server address
    listen(socket_fd, LISTENQ);                                            // Start listening for incoming connections
    printf("Client is listening %d\n", args->port);
    int connectfd = accept(socket_fd, (struct sockaddr *)&cliaddr, &len); // Accept a connection from a client

    char buffer[BUFFER_SIZE];

    while (recv(connectfd, buffer, BUFFER_SIZE, 0) > 0) // Receive a message from the client
    {
        printf("[RECV] %s\n", buffer);
    }

}


int main(int argc, char **argv)
{
    pthread_t h1, h2;                     // thread identifiers
    struct thread_arguments *args, *argr; // pointers to the thread arguments structures


    if (argc != 4) // Check if the number of command-line arguments is correct
    {
        printf("./program IP listenPort sendPort\n"); // Print the correct usage of the program
        return 0;
    }
    else
    {
        printf("IP: %s, listenPort: %s, sendPort: %s\n", argv[1], argv[2], argv[3]);
    }

     argr = (struct thread_arguments *)malloc(sizeof(struct thread_arguments)); // Allocate memory for the receiver thread arguments structure
    argr->port = atoi(argv[2]);                                                // Set the receiver port number
    pthread_create(&h1, NULL, reciever, (void *)argr);                         // Create the receiver thread
    pthread_detach(h1);                                                        // Detach the receiver thread

    sleep(5); // Sleep for 10 seconds
    args = (struct thread_arguments *)malloc(sizeof(struct thread_arguments)); // Allocate memory for the sender thread arguments structure
    strcpy(args->ip, argv[1]);                                                 // Set the sender IP address
    args->port = atoi(argv[3]);                                                // Set the sender port number
    pthread_create(&h2, NULL, sender, (void *)args);                           // Create the sender thread
    pthread_detach(h2);    
    
    while (1) // Infinite loop to keep the program running
    {
    };

    return 0;               
}