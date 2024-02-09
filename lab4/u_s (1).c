#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 3000
#define BUFFER_SIZE 10

struct client_data {
    int temp;
    int num;
    struct sockaddr client_address;
    socklen_t client_length;
};

struct thread_arguments {
    int *sum;
    int *count;
};

static void *handle_client(void *arg) {
    struct client_data *data = (struct client_data *)arg; 
    int temp = data->temp;
    int num = data->num;
    struct sockaddr client_address = data->client_address;
    socklen_t client_length = data->client_length;

    // Free memory used by the client data
    free(data);

    printf("Received temp: %d from client %d\n", temp, num); 
    //inet_ntoa(((struct sockaddr_in *)&client_address)->sin_addr));

    return NULL;
}


int sum = 0;
int count = 0;
int c=0;
    
    
int main(void) {
    int server_fd;
    pthread_t helper; //thread id
    struct sockaddr_in server_address, client_address;
    socklen_t client_length = sizeof(struct sockaddr);

    // Create an IP4/UDP socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Initialize the address of this host
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    bind(server_fd, (struct sockaddr *)&server_address, sizeof(struct sockaddr));
     printf("Server is waiting ....\n"); //waiting for connections
     
    //infinite loop for continuously handling client connections.
    while (1) {
        int valuet; //temp values
        recvfrom(server_fd, &valuet, sizeof(valuet), 0, (struct sockaddr *)&client_address, &client_length);
	
	int valueb; //battery values
        recvfrom(server_fd, &valueb, sizeof(valueb), 0, (struct sockaddr *)&client_address, &client_length);
        
        int value_s; //client identifier
        recvfrom(server_fd, &value_s, sizeof(value_s), 0, (struct sockaddr *)&client_address, &client_length);
	
        if (valuet == -1) {
            count--;
            if (count == 0) {
                // All clients closed, calculate and print the average
                double average = (double)sum / c;
                printf("Average of values: %.2f\n", average);
                break;
            }
        } else {
            // Create a thread to handle the client's value
            struct client_data *data = (struct client_data *)malloc(sizeof(struct client_data));
            data->temp = valuet;
            data->num = value_s;
            memcpy(&(data->client_address), &client_address, client_length);
            data->client_length = client_length;

            sum += valuet;
            count++;
            c++;
            if(valueb < 30){
            	printf(" Low battery ");
            }

            //creates new thread using the function 
            pthread_create(&helper, NULL, handle_client, (void *)data);
            //detaches the newly created thread from the main thread -> allows os to reclain resources allocated when it exits 
            pthread_detach(helper);
        }
    }

    // Close the server socket
    close(server_fd);

    return 0;
}

