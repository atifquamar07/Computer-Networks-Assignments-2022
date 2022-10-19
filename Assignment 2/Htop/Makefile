default: epollServer forkServer pthreadServer pthreadClient pollServer selectServer sequentialServer sequentialClient

epollServer: epoll-server.c
	gcc -o epollServer epoll-server.c

forkServer: fork-server.c
	gcc -o forkServer fork-server.c 

pthreadServer: pThread-server.c
	gcc -o pthreadServer pThread-server.c -lpthread

pthreadClient: pThread-client.c
	gcc -o pthreadClient pThread-client.c -lpthread

pollServer: poll-server.c
	gcc -o pollServer poll-server.c 

selectServer: select-server.c
	gcc -o selectServer select-server.c 

sequentialServer: sequential-server.c
	gcc -o sequentialServer sequential-server.c 

sequentialClient: sequential-client.c
	gcc -o sequentialClient sequential-client.c 

clean:
	rm -f epollServer forkServer pthreadServer pthreadClient pollServer selectServer sequentialServer sequentialClient

