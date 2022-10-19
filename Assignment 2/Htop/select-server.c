#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <pthread.h>

/********************************************************
 **************** Factorial function ********************
/********************************************************/

long long factorial(int n) {

    long long temp = 1;

    for (int i = 1; i <= n; i++){
        temp*=i;
    }
    
    return temp;
}

/********************************************************
 ************* Error checker functions********************
/********************************************************/

int error_check(int fd, const char *message) {

    if(fd == -1){
        perror(message);
        exit(EXIT_FAILURE);
    }
    return fd;
}

int main(int argc, char const *argv[])
{   
    /*************************************************************************
     ************* Initialization of variables and structs********************
    /*************************************************************************/

    int server_socket, client_socket, addr_size, bind_status, listen_status, select_status, file_pointer;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    /********************************************************
	 ************* Initialising server address***************
	/********************************************************/

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(9001);

    /********************************************************
	 ******Socket creation, Bind, Listen commands************
	/********************************************************/

    error_check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket.\n");
    error_check(bind_status = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)), "Binding failed !!!.\n");
    error_check(listen_status = listen(server_socket, 100), "Listen failed.\n");

    /********************************************************
	 ************Creating set of file descriptors************
	/********************************************************/

    fd_set current_sockets, ready_sockets;

    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);

    char buf[50];

    error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

    while(1){
        
        ready_sockets = current_sockets;

        /*****************************************************************************
		 ******Waiting for one of the file descriptors to get open using select()*****
		/*****************************************************************************/

        error_check(select_status = select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL), "Select error\n");

        for (int i = 0; i < FD_SETSIZE; i++){

            /*****************************************************************************
             *********************If socket is ready for a connection*********************
            /*****************************************************************************/
            
            if(FD_ISSET(i, &ready_sockets)){

                if(i == server_socket){

                    /************************************************************************************
                     ********Extracting first connections of pending connections using accept()**********
                    /************************************************************************************/

                    int client_socket;
                    int socket_addr_length = sizeof(client_address);

                    error_check(client_socket = accept(server_socket,(struct sockaddr *)&client_address,&socket_addr_length), "Error in acceptance\n");
                    inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buf, sizeof(client_address));

                    FD_SET(client_socket, &current_sockets);

                }
                else {

                    /********************************************************
                     ************Receiving and sending messages**************
                    /********************************************************/ 

                    int receive_status, send_status;

                    for (int j = 1; j <= 20; j++){   
                        
                        int received_number[1];
                        received_number[0] = -1;

                        error_check(receive_status = recv(i, received_number, sizeof(received_number), 0), "Recv command failed !!!\n");

                        /************************************************************************************
                         ****************************Writing process commented out***************************
                        /************************************************************************************/
                        
                        // char toWrite[512];
                        // char portt[50];
                        // char num[5];
                        // sprintf(portt, "%d", (int)ntohs(client_address.sin_port));
                        // sprintf(num, "%d", (int)received_number[0]);

                        // strcpy(toWrite, "IP: ");
                        // strcat(toWrite, buf);
                        // strcat(toWrite, ", Port: ");
                        // strcat(toWrite, portt);
                        // strcat(toWrite, ", Integer received server side is ");
                        // strcat(toWrite, num);
                        // strcat(toWrite, "\n");

                        // int len = strlen("IP: ") + strlen(buf) + strlen(", Port: ") + strlen(portt) + strlen(", Integer received server side is ");
                        // len += strlen(num) + strlen("\n");

                        // write(file_pointer, &toWrite, len);

                        printf("Integer received on the server side is %d\n", received_number[0]);

                        long long buffer[1];
                        buffer[0] = factorial(received_number[0]);

                        error_check(send_status = send(i, buffer, sizeof(buffer), 0), "Error in sending\n");

                    }

                    FD_CLR(i, &current_sockets);
                }
            }
        }

    }

    return 0;
}

