#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

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
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	perror("cannot create socket");
	printf("socket successfully created!\n");
	printf("socket num: %d\n", fd);
	// have to assign a socket address struct to the socket 
	// ie let's give the phone a number to be reached at/reach others
	// for the ip network we are using, we are assigning a port number
	// other protocols will have different types of "transport addresses"
	// int 
	// bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// the bind needs a sockaddr struct so let's make that first
	
	struct sockaddr_in myaddr;

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(1080);

	int bind_success = -1;
	if ((bind_success = 
		bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0))
		 perror("cannot bind socket to address");
	printf("successfully binded socket to address!\n");
	printf("bind_success: %d\n", bind_success);


	return 0;
}
