#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

//user name n port num stored
char name[30];
int PORT;

void sending();
void receiving(int server_fd);
void *receive_thread(void *server_fd);

int main(int argc, char const *argv[])
{
	printf("Enter name: ");
	scanf("%s",name);
	printf("Enter your port number:");
        scanf("%d", &PORT);
        
        int server_fd, valread,k=0;
        
       struct sockaddr_in address;
       
       if((server_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
        {
        perror("socket failed");
        exit(EXIT_FAILURE);
        }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    //Printed the server socket addr and port
    printf("IP address is: %s\n", inet_ntoa(address.sin_addr));
    printf("port is: %d\n", (int)ntohs(address.sin_port));
    
    //binding socket to addr n port specified
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if(listen(server_fd,5)<0)
    {
    	perror("listen");
    	exit(EXIT_FAILURE);
    }
    
    int ch;
    pthread_t tid;
    pthread_create(&tid, NULL, &receive_thread, &server_fd);
    //Creating thread to keep receiving message in real time
   
   
    printf("\n*****in ur choice enter the following:*****\n1. Send message\n0. Quit\n");
    printf("\nEnter ur choice:");
    
    do
    {
        scanf("%d", &ch);
        switch (ch)
        {
        case 1:
            sending();
            break;
        case 0:
            printf("\nLeaving\n");
            break;
        default:
            printf("\nWrong choice\n");
        }
    } while (ch);
    close(server_fd);
    return 0;

}

//sends msg to peer using tcp socket connection
void sending()
{
      //buffer toi store msg to be sent
	char buffer[2000]={0};
	
	// fetching port num to which msg sent
	int PORT_server;
	
	//consider each peer will enter diff port
	printf("Enter the port to send messsge: ");
	scanf("%d", &PORT_server);
	
	int sock=0, valread;
	struct sockaddr_in serv_addr;
	
	 char hel[1024] = {0};
    
    //creating socket fd 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY always gives an IP of 0.0.0.0
    serv_addr.sin_port = htons(PORT_server);
    
    //establish connection
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    
    char dummy;
    printf("Enter your message:");
    scanf("%c", &dummy); 
    //read msg to be sent from user till newline n store in hello 
    //dummy is readin only single char to avoid issues with scanf calls
    
    scanf("%[^\n]s", hel);
    
    //adding senders name n port no n storing formatted str in buffer
    sprintf(buffer, "%s[PORT:%d] says: %s", name, PORT, hel);
    
    send(sock, buffer, sizeof(buffer), 0);
    printf("\nMessage sent\n");
    close(sock);
}
    
//Calling receiving every 2 seconds
//continuosly check for incoming msg from other peers in real time
void *receive_thread(void *server_fd)
{
    int s_fd = *((int *)server_fd);
    while (1)
    {
        sleep(2);//pause for 2 sec 
        receiving(s_fd);
    }
}


//Receiving messages on our port
void receiving(int server_fd)
{
    struct sockaddr_in address; //store client addr
    int valread; //store no of bytes read
    char buffer[2000] = {0}; // store recieved msgs
    int addrlen = sizeof(address); //store the size of the addr
    
    
    fd_set current_sockets, ready_sockets;
    //current->store the set of fd tht are currently being monitored for activity
    //ready->store the set of fd tht are ready for read opn
    
    //sets up the set of fd to monitor for incoming msgs
    FD_ZERO(&current_sockets); //innitalize fd_set structure by clearing all fd from set
    FD_SET(server_fd, &current_sockets); // set current socket as server socket
    //fds added to fd_set using FD_SET
    
    
    int k = 0;
    while (1)
    {
        k++;
        ready_sockets = current_sockets;// fd ready for reading using select
        //ready is temp
        
        
        //select allow prog to effiecinerly wait for activity on multiple fd
        //select(nfds,readfds,writefds,exceptfds,timeout)
        //returns the total no of fds tht are ready
        
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }
        
        //iterators through each fd in ready_sockets set to determine which socket is ready for reading
        for (int i = 0; i < FD_SETSIZE; i++)
        {
        //FDIISET ->CHECK IF FD IS SET IN AN fd_set
            if (FD_ISSET(i, &ready_sockets))
            {
                if (i == server_fd)//fd == server socket -> ther is incoming connection request
                {
                    int client_socket;
                    //creates new client socket n addded to current sockets set
                    if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                                (socklen_t *)&addrlen)) < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(client_socket, &current_sockets);
                }
                else //incoming data in one of the clients
                {
                    valread = recv(i, buffer, sizeof(buffer), 0);//recieves the data
                    printf("\n%s\n", buffer);
                    FD_CLR(i, &current_sockets);//removes client socket from current sockets
                }
            }
        }
        if (k == (FD_SETSIZE * 2)) // maxm no of fds tht can be stored in fd_set       
            break;
    }
}
