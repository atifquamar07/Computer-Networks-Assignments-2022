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

int error_check(int exp, const char *msg){

    if(exp == -1){
        perror(msg);
        exit(1);
    }
    return EXIT_FAILURE;
}
int error_check1(int exp, const char *msg){

    if(exp == -1){
        exit(1);
    }
    return EXIT_FAILURE;
}




int main(int argc, char const *argv[]){

    /*************************************************************************
	 ************* Initialization of variables and structs********************
	/*************************************************************************/

    int serverSocket, bind_status, listen_status, thread_creation, newSocket, file_pointer;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    /********************************************************
	 ************* Initialising server address***************
	/********************************************************/

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9001);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /********************************************************
	 ******Socket creation, Bind, Listen commands************
	/********************************************************/

    error_check(serverSocket = socket(AF_INET, SOCK_STREAM, 0), "Error creating socket!!!.\n");
    error_check(bind_status = bind(serverSocket, (struct sockaddr *)&server_address, sizeof(server_address)), "Binding failed !!!.\n");
    error_check(listen_status = listen(serverSocket, 300), "Listening command failed\n");

    /********************************************************
	 *************Opening file in write mode*****************
	/********************************************************/

    error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

    /********************************************************
	 ******Child process creation variable initialized*******
	/********************************************************/

    pid_t childpid;

    /********************************************************
	 ******************Client address size*******************
	/********************************************************/

    int socket_addr_length = sizeof(client_address);
    char buf[50];

    while(1){

        /************************************************************************************
         ********Extracting first connections of pending connections using accept()**********
        /************************************************************************************/
        
        error_check1(newSocket = accept(serverSocket, (struct sockaddr *)&client_address,&socket_addr_length), "Accept command failed!!!\n");
        inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buf, sizeof(client_address));

        /********************************************************
         ****************Child process creation******************
        /********************************************************/

        if((childpid = fork()) == 0){

            close(serverSocket);

            for (int i = 1; i <= 20; i++){ 

                /********************************************************
				 ************Receiving and sending messages**************
				/********************************************************/ 

                int receive_status, send_status; 

                int received_number[1];
                received_number[0] = -1;

                error_check(receive_status = recv(newSocket, received_number, sizeof(received_number), 0), "Receive command failed !!!\n");

                /********************************************************
                 ****************Writing to file process*****************
                /********************************************************/

                char toWrite[512];
                char portt[50];
                char num[5];
                sprintf(portt, "%d", (int)ntohs(client_address.sin_port));
                sprintf(num, "%d", (int)received_number[0]);

                strcpy(toWrite, "IP: ");
                strcat(toWrite, buf);
                strcat(toWrite, ", Port: ");
                strcat(toWrite, portt);
                strcat(toWrite, ", Integer received server side is ");
                strcat(toWrite, num);
                strcat(toWrite, "\n");

                int len = strlen("IP: ") + strlen(buf) + strlen(", Port: ") + strlen(portt) + strlen(", Integer received server side is ");
                len += strlen(num) + strlen("\n");

                write(file_pointer, &toWrite, len);

                printf("Integer received on the server side is %d\n", received_number[0]);

                long long buffer[1];
                buffer[0] = factorial(received_number[0]);

                error_check(send_status = send(newSocket, buffer, sizeof(buffer), 0), "Send command failed !!!\n");

            }
        }

    }

    return 0;
}

