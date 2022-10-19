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

int error_check(int fd, const char *message){

    if(fd == -1){
        perror(message);
        exit(EXIT_FAILURE);
    }
    return fd;
}

/*************************************************************************
 ************* Initialization of variables and structs********************
/*************************************************************************/

int file_pointer;

struct sockaddr_in server_address;
struct sockaddr_in client_address;
int socket_addr_length = sizeof(client_address);
char buf[50];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*************************************************************************
 ********Function to be performed by each dispatched thread***************
/************************************************************************/

void *thread_computation(void *ptr){

    int *temp = (int *)ptr;
    int newSocket = *temp;

    for (int i = 1; i <= 20; i++){  

        int receive_status, send_status; 

        int received_number[1];
        received_number[0] = -1;
        
        pthread_mutex_lock(&lock);

        error_check(receive_status = recv(newSocket, received_number, sizeof(received_number), 0), "Receive command failed !!!\n");

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

        pthread_mutex_unlock(&lock);

    }

    close(newSocket);

}


int main(){

    /*************************************************************************
	 ************* Initialization of variables and structs********************
	/*************************************************************************/

    int serverSocket, bind_status, listen_status, newSocket, thread_creation;

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
	 ***************Array to store each thread***************
	/********************************************************/ 

    pthread_t tid[300];

    /********************************************************
	 *************Opening file in write mode*****************
	/********************************************************/

    error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

    int i = 0;

    while (1){

        /************************************************************************************
         ********Extracting first connections of pending connections using accept()**********
        /************************************************************************************/

        error_check(newSocket = accept(serverSocket, (struct sockaddr *)&client_address,&socket_addr_length),"Accept command failed !!!.\n");
        inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buf, sizeof(client_address));

        /**********************************************************************************************
         *************Creating a thread to handle client requests and dispatching them*****************
        /*********************************************************************************************/

        error_check(thread_creation = pthread_create(&(tid[i]), NULL, thread_computation, &newSocket), "Failed to create thread !!!\n");
        pthread_join(tid[i++], NULL);

            
    }


    return 0;

}