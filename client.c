// client.c
//
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
/*
 * Guidance from Rutgers University:
 * https://www.cs.rutgers.edu/~pxk/rutgers/notes/sockets/index.html
 *
 */

// TODO: 
// step 3a. connect to a server from client


int main(){
	uint16_t port_num = 8082;
	// name is either a hostname or an IPv4 address in standard dot notation	
	// if name is a hostname then the hostname will be copied into the 
	// aliases value of struct and then the IPv4 will be searched for & stored
	const char *host_name = "localhost";
	uint32_t name_t = 0;	

	printf("hello world\n");
	// here we're going to create a socket
	// it's like you just bought a phone, but it doesn't have a number yet
	// that's what bind() is for ;)
	int fd = -1;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
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
	
	struct sockaddr_in myaddr;

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	int bind_success = -1;
	if ((bind_success = 
	bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)){
		perror("cannot bind socket to address");
		exit(1);
	}
	printf("successfully binded socket to address!\n");
	printf("bind_success: %d\n", bind_success);
	
	// TODO: 
	// step 3a. connect to a server from client
	// going to use the connect function here
	// we created a socket that knows where it's grounded
	// now we have to tell it where its connecting to
	// ie we gave the phone a landline, but now we have to 
	// tell it which number to dial
	
	//int
	//connect(int socket, const struct sockaddr *addr, socklen_t address_len);
	
	// for the second parameter we have to create a struct sockaddr
	// that tells the socket where to connect to, or the "transport address"
	
	struct hostent *hp;     /* host information */
	struct sockaddr_in servaddr;    /* server address */

	/* fill in the server's address and data */
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_num);
	
	hp = gethostbyname(host_name);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", host_name);
		return 0;
	}	

	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
	
	printf("host name: %s\n", hp->h_name);
	//char *inet_ntoa(struct in_addr in);

	printf("host name: %s\n", inet_ntoa(servaddr.sin_addr));
	
	int connect_success = -1;
	/* connect to server */
	if ((connect_success = connect(fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in))) < 0) {
		printf("connect_success: %d\n", connect_success);
		perror("connect failed");
		exit(1);
	}
	printf("connect_success: %d\n", connect_success);
	char buffIn[256];
	char buffRead[256];
	char *exitStr = "exit\n";
	// write
	printf("enter input: ");
	fgets(buffIn,256,stdin);
	printf("input: %s", buffIn);
	int numBytesWritten = -1;
	//numBytesWritten = write(fd, buffSend, 20);
	numBytesWritten = write(fd, buffIn, 256);
	printf("write success: %d\n", numBytesWritten);
	int strcmpVal = strcmp(buffIn, exitStr);
	printf("strcmpVal: %d\n", strcmpVal);
	int numBytesRead;
	// read
	bzero(buffRead, sizeof(buffRead));
	numBytesRead = 0;
	printf("ready to read\n");
	numBytesRead = read(fd, buffRead, 256);
	printf("buffRead: %s\n", buffRead);
	if (numBytesRead == -1){
		perror("read error");
		exit(1);
	}
	while (strcmpVal != 0){
		fflush(stdin);
		printf("enter input: ");
		fgets(buffIn,256,stdin);
		printf("input: %s", buffIn);
		//char *buffSend = "hell0 dere do";
		numBytesWritten = -1;
		//numBytesWritten = write(fd, buffSend, 20);
		numBytesWritten = write(fd, buffIn, 256);
		printf("write success: %d\n", numBytesWritten);
		strcmpVal = strcmp(buffIn, exitStr);
		//printf("strcmpVal: %d\n", strcmpVal);
		//printf("exitStr: %s", exitStr);
		//printf("buffIn: %s", buffIn);
		bzero(buffRead, sizeof(buffRead));
		numBytesRead = 0;
		printf("ready to read\n");
		numBytesRead = read(fd, buffRead, 256);
		printf("buffRead: %s\n", buffRead);
		if (numBytesRead == -1){
			perror("read error");
			exit(1);
		}
	}
	int shutdown_success = -1;
	shutdown_success = shutdown(fd, SHUT_RDWR);
	printf("shutdown success: %d\n", shutdown_success);

	return 0;
}

