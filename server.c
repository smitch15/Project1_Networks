#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Guidance from Rutgers University:
 * https://www.cs.rutgers.edu/~pxk/rutgers/notes/sockets/index.html
 *
 */

int main(){
	printf("hello world\n");
	// here we're going to create a socket
	// it's like you just bought a phone, but it doesn't have a number yet
	// that's what bind() is for ;)
	int fd = -1;
	int sockoptval = 1;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){	
		perror("cannot create socket");
		exit(1);
	}
	printf("socket successfully created!\n");
	printf("socket num: %d\n", fd);
	// have to assign a socket address struct to the socket 
	// ie let's give the phone a number to be reached at/reach others
	// for the ip network we are using, we are assigning a port number
	// other protocols will have different types of "transport addresses"
	// int 
	// bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// the bind needs a sockaddr struct so let's make that first
	
	// set this so that when the socket closes it immediately reopens
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	struct sockaddr_in myaddr;
	
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(1080);

	//printf("IPv4 address: %u\n", myaddr.sin_addr.s_addr);
	
	int bind_success = -1;
	if ((bind_success = 
		bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)){
		 perror("cannot bind socket to address");
		 exit(1);
	}
	printf("successfully binded socket to address!\n");
	printf("bind_success: %d\n", bind_success);
	
	/* set the socket for listening (queue backlog of 15) */
	if (listen(fd, 6) < 0) {
		perror("listen error");
		exit(1);
	}

	socklen_t client_addr_len;
	struct sockaddr_in client_addr;
	int rqst;

	// loop until you accept connections from incoming clients
	while (1) {
		printf("entering while loop\n");
		rqst = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);	
		printf("fd: %d\n", fd);
		printf("rqst: %d\n", rqst);
		printf("accept returned...\n");
		while (rqst < 0) {
            printf("client address length %d\n\n", client_addr.sin_len);
			if ((errno != ECHILD) && (errno != EINTR)) {
            	perror("accept failed");
                exit(1);
        	} 
        }
		perror("error with accept");
		exit(1);
	}

	return 0;
}
