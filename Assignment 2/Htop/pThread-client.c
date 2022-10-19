#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <pthread.h>
#include <errno.h>

pthread_mutex_t lockk = PTHREAD_MUTEX_INITIALIZER;

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

/*************************************************************************
 ********Function to be performed by each dispatched thread***************
/************************************************************************/

void *clientThread(void *arg){

    /*************************************************************************
	 ************* Initialization of variables and structs********************
	/*************************************************************************/

    int clientSocket, connect_status;
    struct sockaddr_in serverAddr;

    /********************************************************
	 ************* Initialising server address***************
	/********************************************************/

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9001);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    
    /********************************************************
	 ***********Socket creation, Connect commands************
	/********************************************************/

    error_check(clientSocket = socket(AF_INET, SOCK_STREAM, 0), "Socket command failed !!!\n");
    error_check(connect_status = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)), "Connection command failed !!!\n");


    /*********************************************************************
	 *********Sending 20 integers and receiving the factorials************
	/*********************************************************************/

    for (int i = 1; i <= 20; i++){
        
        int data_packet[1];
        data_packet[0] = i;

        error_check(send(clientSocket, data_packet, sizeof(data_packet), 0), "Send command failed\n");

        printf("Integer %d sent from client side.\n", data_packet[0]);
     
        long long received_number[1];
        received_number[0] = -1;

        error_check(recv(clientSocket, received_number, sizeof(received_number), 0), "Receive command failed.\n");

        printf("The factorial received here is %lld\n\n", received_number[0]);
        
    }

    close(clientSocket);
    pthread_exit(NULL);

}

int main(int argc,char *argv[]){  

    /********************************************************
	 ***************Array to store each thread***************
	/********************************************************/ 

    pthread_t tid[100];

    int thread_create;

    /*****************************************************************
	 ***************Creating and dispatching 10 threads***************
	/*****************************************************************/ 

    for (int i = 1; i <= 10; i++){

        printf("*******************************\n");
        printf("Iteration Number %d\n", i);
        printf("*******************************\n\n");

        error_check(thread_create = pthread_create(&(tid[i]), NULL, clientThread, NULL), "Pthread_create command failed\n");

        pthread_join(tid[i], NULL);
        
    }

    return 0;
}