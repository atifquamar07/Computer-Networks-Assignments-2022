//************************************ CLIENT *************************************//

#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>

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
  
int main(int argc, char const* argv[])
{      
    /*************************************************************************
     ************* Initialization of variables and structs********************
    /*************************************************************************/

    int socket_ID, connectStatus, send_status, receive_status;
    struct sockaddr_in client_address;

    /********************************************************
	 ************* Initialising server address***************
	/********************************************************/
  
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(9001); 
    client_address.sin_addr.s_addr = INADDR_ANY;

    /********************************************************
	 ***********Socket creation, Connect commands************
	/********************************************************/

    error_check(socket_ID = socket(AF_INET, SOCK_STREAM, 0), "Error in socket generation from client side !!!!\n");
    error_check(connectStatus = connect(socket_ID, (struct sockaddr*)&client_address, sizeof(client_address)), "Error in connection from client side !!!!\n");

    /*********************************************************************
	 *********Sending 20 integers and receiving the factorials************
	/*********************************************************************/

    for (int i = 1 ; i <= 20 ; i++){

        int data_packet[1];
        data_packet[0] = i;
        long long buffer[1];
        buffer[0] = -1;

        printf("Integer %d sent from client side.\n", i);

        error_check(send_status = send(socket_ID, data_packet, sizeof(data_packet), 0), "Data packet sending failed !!!!\n");
        error_check(receive_status = recv(socket_ID, buffer, sizeof(buffer), 0), "Error in reading data from client side !!!!\n");

        printf("The factorial received on the client side is %lld\n", buffer[0]);
        printf("\n");
 
    }

    close(socket_ID);
    exit(EXIT_SUCCESS);

    return 0;
}