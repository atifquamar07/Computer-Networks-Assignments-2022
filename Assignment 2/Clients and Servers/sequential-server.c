//************************************ SERVER *************************************//

#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/********************************************************
 **************** Factorial function ********************
/********************************************************/

long long factorial(int n){

    if(n == 0 || n == 1){
        return n;
    }

    return n*factorial(n-1);
}

/********************************************************
 ************* Error checker functions********************
/********************************************************/

int error_check(int exp, const char *msg){

    if(exp == -1){
        perror(msg);
        exit(1);
    }
    return EXIT_FAILURE;
}


int main(int argc, char const* argv[])
{   

    /*************************************************************************
     ************* Initialization of variables and structs********************
    /*************************************************************************/

	int server_socket_ID, bind_status, listen_status, accept_status, file_pointer;
	struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    /********************************************************
	 ************* Initialising server address***************
	/*******************************************************/

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9001);
	server_address.sin_addr.s_addr = INADDR_ANY;

    /********************************************************
	 ******Socket creation, Bind, Listen commands************
	/********************************************************/

    error_check(server_socket_ID = socket(AF_INET, SOCK_STREAM, 0), "Socket error !!! \n");
	error_check(bind_status = bind(server_socket_ID, (struct sockaddr*)&server_address, sizeof(server_address)), "Binding failed !!!!\n");
	error_check(listen_status = listen(server_socket_ID, 1), "Listening failed");

    char address[1024];

    int socklen = sizeof(client_address);

    /********************************************************
	 ************Creating set of file descriptors************
	/********************************************************/

    error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

    while(1){

        /************************************************************************************
         ********Extracting first connections of pending connections using accept()**********
        /************************************************************************************/

        error_check(accept_status = accept(server_socket_ID, (struct sockaddr *)&client_address, &socklen), "Acceptance failed !!!!\n");
        inet_ntop(AF_INET, (char *)&(client_address.sin_addr), address, sizeof(client_address));

        /********************************************************
        *************Receiving and sending messages**************
        /********************************************************/ 

        for (int j = 1; j <= 20; j++){

            int buffer[1];
            buffer[0] = -1;
            long long data_packet[1];
            data_packet[0] = -1;

            int connectStatus = recv(accept_status, buffer, sizeof(buffer), 0);

            /********************************************************
             ****************Writing to file process*****************
            /********************************************************/

            char toWrite[512];
            char portt[50];
            char num[5];
            sprintf(portt, "%d", (int)ntohs(client_address.sin_port));
            sprintf(num, "%d", (int)buffer[0]);

            strcpy(toWrite, "IP: ");
            strcat(toWrite, address);
            strcat(toWrite, ", Port: ");
            strcat(toWrite, portt);
            strcat(toWrite, ", Integer received server side is ");
            strcat(toWrite, num);
            strcat(toWrite, "\n");

            int len = strlen("IP: ") + strlen(address) + strlen(", Port: ") + strlen(portt) + strlen(", Integer received server side is ");
            len += strlen(num) + strlen("\n");

            write(file_pointer, &toWrite, len);

            printf("Integer received on the server side is %d\n", buffer[0]);

            if (connectStatus == -1) {
                printf("Error in reading data from client side !!!!\n");
                exit(EXIT_FAILURE);
            }

            
            data_packet[0] = factorial(buffer[0]);

            bind_status = send(accept_status, data_packet, sizeof(data_packet), 0);

            if (bind_status == -1){
                printf("Data packet sending failed !!!!\n");
            }

        }

        close(server_socket_ID);
        break;

    }

    exit(EXIT_SUCCESS);
	return 0;


}
