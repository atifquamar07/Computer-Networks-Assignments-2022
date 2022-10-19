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
#include <arpa/inet.h>
#include <sys/stat.h>


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
 ************* Error checker function********************
/********************************************************/

int error_check(int fd, const char *message) {

    if(fd == -1){
        perror(message);
        exit(EXIT_FAILURE);
    }
    return fd;
}


int main(int argc, char const *argv[]){

	/*************************************************************************
	 ************* Initialization of variables and structs********************
	/*************************************************************************/

	int epoll_file_desc, epoll_wait_status, socket_descriptor, bind_status, non_blocking_status, socket_addr_length;
	int connect_status,listen_status, epoll_add, epoll_ctl_status, receive_status, send_status, file_pointer;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	struct epoll_event events[300];

	/********************************************************
	 *******************Socket opening***********************
	/********************************************************/
	error_check(socket_descriptor = socket(AF_INET, SOCK_STREAM, 0), "Socket connection failed\n");

	/********************************************************
	 ************* Initialising server address***************
	/********************************************************/

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(9001);

	/********************************************************
	 ******Bind, Non-Blocking call, listen commands**********
	/********************************************************/
	
	error_check(bind_status = bind(socket_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)), "Binding failed !!!\n");
	error_check(non_blocking_status = fcntl(socket_descriptor, F_SETFD, fcntl(socket_descriptor, F_GETFD, 0) | O_NONBLOCK), "Non blocking error\n");
	error_check(listen_status = listen(socket_descriptor, 300), "Error in listening\n");

	epoll_file_desc = epoll_create(1);

	struct epoll_event temp;
	temp.events = EPOLLIN | EPOLLOUT | EPOLLET;
	temp.data.fd = socket_descriptor;

	error_check(epoll_ctl_status = epoll_ctl(epoll_file_desc, EPOLL_CTL_ADD, socket_descriptor, &temp), "Error in epoll_ctl\n");

	/********************************************************
	 ******************Client address size*******************
	/********************************************************/

	socket_addr_length = sizeof(client_address);
	char buf[50];

	/********************************************************
	 *************Opening file in write mode*****************
	/********************************************************/

	error_check(file_pointer = open("output.txt", O_WRONLY), "Error in opening file\n");

	/********************************************************
	 ********************Server is on************************
	/********************************************************/

	while(1) {

		error_check(epoll_wait_status = epoll_wait(epoll_file_desc, events, 300, -1), "Error in epoll wait\n");

		for (int i = 0; i < epoll_wait_status; i++) {

			if (events[i].data.fd == socket_descriptor) {

				/************************************************************************************
				 ********Extracting first connections of pending connections using accept()**********
				/************************************************************************************/

				error_check(connect_status = accept(socket_descriptor,(struct sockaddr *)&client_address,&socket_addr_length), "Error in acceptance\n");

				inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buf, sizeof(client_address));

				error_check(non_blocking_status = fcntl(connect_status, F_SETFD, fcntl(connect_status, F_GETFD, 0) | O_NONBLOCK), "Non blocking error\n");

				struct epoll_event new_con;
				new_con.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
				new_con.data.fd = connect_status;

				if (epoll_ctl(epoll_file_desc, EPOLL_CTL_ADD, connect_status, &new_con) == -1) {
					perror("epoll_ctl()\n");
					exit(1);
				}
				
			} else if (events[i].events & EPOLLIN) {

				/********************************************************
				 ************Receiving and sending messages**************
				/********************************************************/
	
                int received_number[1];
                received_number[0] = -1;

				error_check(receive_status = recv(events[i].data.fd, received_number, sizeof(received_number), 0), "Receive command failed\n");

				if(received_number[0] != -1){

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

					error_check(send_status = send(events[i].data.fd, buffer, sizeof(buffer), 0), "Error in send command\n");

				}  
			} 
		}
	}
    return 0;
}

