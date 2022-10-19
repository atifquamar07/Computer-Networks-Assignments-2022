#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<poll.h>
#include <fcntl.h> 
#include <unistd.h>

#define ZERO 0
#define ERROR -1

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


int poll_error_check(int exp, const char *msg1, const char *msg2){

    if(exp == -1){
        perror(msg1);
    }
	else if(exp == 0){
		perror(msg2);
	}
	return exp;
}


int main(){

    /*************************************************************************
	 ************* Initialization of variables and structs********************
	/*************************************************************************/

	int sockfd, bind_status, listen_status, file_pointer;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	char buf[50];

	/********************************************************
	 ************* Initialising server address***************
	/********************************************************/

	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9001);
	server_address.sin_addr.s_addr = INADDR_ANY;

	/********************************************************
	 ******Socket creation, Bind, Listen commands************
	/********************************************************/

	error_check(sockfd = socket(AF_INET,SOCK_STREAM,0), "Error in socket connection\n");
	error_check(bind_status = bind(sockfd,(struct sockaddr *)&server_address,sizeof(server_address)), "Error in binding\n");
	error_check(listen_status = listen(sockfd,200), "Error in listening\n");

	/********************************************************
	 *************File descriptors array*********************
	/********************************************************/

	struct pollfd file_descriptors[300];

	/********************************************************
	 ***** Initialization of File descriptors array**********
	/********************************************************/

	for(int idx1 = 0 ; idx1 < 300 ; idx1++){

		file_descriptors[idx1].revents = ZERO;
		file_descriptors[idx1].events = ZERO;
		file_descriptors[idx1].fd = ERROR;

	}

	for(int idx2 = 0 ; idx2 < 300 ; idx2++){

		if(file_descriptors[idx2].fd == ERROR){

			file_descriptors[idx2].fd = sockfd;
			file_descriptors[idx2].events = POLLIN;
			file_descriptors[idx2].revents = ZERO;
			break;

		}
	
	}

	/********************************************************
	 *************Opening file in write mode*****************
	/********************************************************/

	error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

	for( ;; ){

		int poll_check;

		/*****************************************************************************
		 ******Waiting for one of the file descriptors to get open using poll()*******
		/*****************************************************************************/

		poll_error_check(poll_check = poll(file_descriptors, 30, 10000), "Error in polling\n", "Time Out\n");

		/*****************************************************************************
		 **********************If empty, we proceed further***************************
		/*****************************************************************************/
		
		if(poll_check > 0) {
			
			for(int i = 0 ; i < 300 ; i++){

				if(file_descriptors[i].fd==-1){
					continue;
				}
				
				if(file_descriptors[i].revents &POLLIN){

					if(file_descriptors[i].fd == sockfd){

						int len = sizeof(client_address);

						/************************************************************************************
						 ********Extracting first connections of pending connections using accept()**********
						/************************************************************************************/
						
						int accept_status = accept(sockfd, (struct sockaddr*)&client_address, &len);
						inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buf, sizeof(client_address));

						if(accept_status < 0){
							continue;
						}

						int idx3 = 0;

						while(idx3 < 300){

							if(file_descriptors[idx3].fd == -1){
								file_descriptors[idx3].fd = accept_status;
								file_descriptors[idx3].events = POLLIN;
								file_descriptors[idx3].revents = 0;
								break;
							}
							idx3++;
						}
					}
					else {

						/********************************************************
						 ************Receiving and sending messages**************
						/********************************************************/ 

                        int received_number[1];
                        received_number[0] = -1;

                        int receive_status = recv(file_descriptors[i].fd, received_number, sizeof(received_number), 0);

                        if(receive_status <= 0){

                            close(file_descriptors[i].fd);

							int idx4 = 0;

							while(idx4 < 300){

								if(file_descriptors[i].fd==file_descriptors[i].fd){
									file_descriptors[i].fd=-1;
									file_descriptors[i].events=0;
									file_descriptors[i].revents=0;
									break;
								}

								idx4++;

							}
                            break;
                        }

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

						int send_status;
						error_check(send_status = send(file_descriptors[i].fd, buffer, sizeof(buffer), 0), "Send command failed !!!\n");

					}
				}
			}
		}
	}

	return 0;

}


