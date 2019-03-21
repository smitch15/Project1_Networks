// server.c
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

/*
 * Guidance from Rutgers University:
 * https://www.cs.rutgers.edu/~pxk/rutgers/notes/sockets/index.html
 *
 */

int main(int argc, char *argv[]){
		//printf("hello world\n");
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
		//printf("socket num: %d\n", fd);
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
		const char *server_address = argv[1];//"192.168.1.5";
		printf("server address : %s\n", argv[1]);
		const char *server_address_b = "127.0.0.1";
		memset((char *)&myaddr, 0, sizeof(myaddr));
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = inet_addr(server_address);
		//myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = htons(8082);

		//printf("Server address: %s\n", inet_ntoa(myaddr.sin_addr));
		char host_name[256];
		gethostname(host_name, sizeof(host_name));

		printf("Server name: %s\n", host_name);
		int bind_success = -1;
		if ((bind_success = 
								bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)){
				perror("cannot bind socket to address");
				exit(1);
		}
		printf("successfully binded socket to address!\n");
		printf("bind_success: %d\n", bind_success);

		/* set the socket for listening (queue backlog of 10) */
		if (listen(fd, 10) < 0) {
				perror("listen error");
				exit(1);
		}

		socklen_t client_addr_len;
		struct sockaddr_in client_addr;
		int rqst;

		int addr_len = sizeof(client_addr);
		struct sockaddr_in sa;
		int len=256;
		char buffer[len];
		int valread;
		//set of socket descriptors  
		fd_set readfds;   
		int max_clients = 10;
		int client_socket[max_clients];
		int sd;
		int i;
		int activity;
		//initialise all client_socket[] to 0 so not checked  
		for (i = 0; i < max_clients; i++)	client_socket[i] = 0;   

		//inet_ntop(AF_INET, &(sa.sin_addr), buffer, len);
		//printf("address:%s\n",buffer);
		int strcmpVal = -1;
		char buffRead[256];
		int numBytesRead;
		char buffWrite[256];
		char *exitStr = "exit\n";
		int numBytesWritten = -1;
		int max_sd;
		printf("before the loop\n\n\n");
		struct pollfd pfd;
		int ret;

		// loop until you accept connections from incoming clients
		while (strcmpVal != 0){
				FD_ZERO(&readfds);   
				FD_SET(fd, &readfds);   
				max_sd = fd; 
				//printf("inside the loop\n\n\n");
				for ( i = 0 ; i < max_clients ; i++){   
						//socket descriptor  
						//printf("inside client loop: %d\n", i);
						sd = client_socket[i];   

						//printf("inside client loop: %d\n", i);
						//if valid socket descriptor then add to read list  
						if(sd > 0) {
								printf("\n\n\n\nfound valid socketASDFASDF descriptor\n\n\n\n");
								FD_SET( sd , &readfds);   
						}
						//printf("inside client loop: %d\n", i);
						//highest file descriptor number, need it for the select function  
						if(sd > max_sd)   
								max_sd = sd;   
						//printf("inside client loop: %d\n\n", i);
				}  
				//printf("gets before declaring timeval");
				struct timeval timeout_select;
				timeout_select.tv_sec = 3;
				activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);   
				printf("THERE IS ACTIVITY FROM %d SOCKETS CURRENTLY\n\n\n\n", activity);
				//printf("activity val from select: %d\n\n", activity);
				//printf("FD_ISSET(fd, &readfds): %d\n", FD_ISSET(fd, &readfds));
				if ((activity < 0) && (errno!=EINTR))   
						perror("select error");   
				// if fd is connecting to an incoming socket connection
				if (FD_ISSET(fd, &readfds)){
						printf("fd is set!! and connecting to a new socket!!!!\n");
						// accept
						// accept the client address and return rqst socket to R/W with client
						if ((rqst = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
								printf("accept finished\n");
								printf("request: %d\n", rqst);
								if ((errno != ECHILD) && (errno != EINTR)) {
										perror("accept failed");
										exit(1);
								} 
						}
						// inet_ntop puts address into a readable buffer string 
						inet_ntop(AF_INET, &(client_addr.sin_addr), buffer, len);
						printf("address:%s\n",buffer);
						// add new file descriptor to the set
						FD_SET(rqst, &readfds);
						if (rqst > max_sd) {
								max_sd = rqst;
						}
						
						// read from client
						bzero(buffRead, sizeof(buffRead)); 
						numBytesRead = 0;
						printf("ready to read\n");
						numBytesRead = read(rqst, buffRead, 256);
						if (numBytesRead == -1){
								perror("read error");
								exit(1);
						}
						//printf("numBytesRead: %d\n", numBytesRead);
						//printf("read buffer: %s", buffRead);
						if (numBytesRead > 0){
								// write to client only if read one message
								fflush(stdin);
								printf("enter input: ");
								fgets(buffWrite,256,stdin);
								//printf("input: %s", buffWrite);
								//char *buffSend = "hell0 dere do";
								numBytesWritten = -1;
								numBytesWritten = write(rqst, buffWrite, 256);
								if (numBytesWritten < 0)	perror("write error");
								printf("write success: %d\n", numBytesWritten);
								strcmpVal = strcmp(buffWrite, exitStr);
						}					
						for (i = 0; i < max_clients; i++){   
								//if position is empty add socket to client socket array 
								if(client_socket[i] == 0 ){   
										client_socket[i] = rqst;   
										//printf("RQST ADDED TO LIST OF CLIENT SOCKETS: %d\n\n\n" , rqst);   
										break;   
								} 
						}
				}
				//printf("start");
				// check all clients for if they are active, if they are, then read or write
				for (i = 0; i < max_clients; i++){   
						sd = client_socket[i];   
						//printf("1");
						if (sd != 0)	printf("active sd: %d\n", sd);
						//printf(" FD_ISSET(sd, &readfds) is: %d\n", FD_ISSET(sd, &readfds));
						if (FD_ISSET(sd, &readfds)){   
								//Check if it was for closing , and also read the  
								//printf("the sd that is set is: %d\n", sd);
								// disconnection from client code here
								printf("CLIENT SD IS SET\n");

								// read from client
								bzero(buffRead, sizeof(buffRead)); 
								numBytesRead = 0;
								printf("ready to read\n");
								// wait for a read operation, unless it doesn't come continue
								pfd.fd = sd; // your socket handler 
								pfd.events = POLLIN;
								ret = poll(&pfd, 1, 100); // .1 second for timeout
								switch (ret) {
									case -1:
										// Error
										perror("error with poll timeout\n");
										break;
									case 0:
										// Timeout 
										printf("timed out\n");
										break;
									default:
										numBytesRead = read(sd, buffRead, 256);
										break;
								}	
								if (numBytesRead == -1){
										perror("read error");
										exit(1);
								}
								printf("numBytesRead: %d\n", numBytesRead);
								// if something is read, then write back, unless socket is closed
								// then handle the closed socket
								if (numBytesRead > 0){
										printf("read buffer: %s", buffRead);
										if ((strcmp(buffRead, exitStr)) == 0 ){
												printf("client socket successfully closed\n\n");
												FD_CLR(sd, &readfds);
												int close_success = close(sd);  
												if (close_success < 0){
														perror("closing error");
														exit(1);
												} 
												client_socket[i] = 0;   
										}else{ 
												fflush(stdin);
												printf("enter input: ");
												fgets(buffWrite,256,stdin);
												printf("input: %s", buffWrite);
												//char *buffSend = "hell0 dere do";
												numBytesWritten = -1;
												//numBytesWritten = write(fd, buffSend, 20);
												numBytesWritten = write(sd, buffWrite, 256);
												printf("write success: %d\n", numBytesWritten);
												strcmpVal = strcmp(buffWrite, exitStr);
										}
								}
						}   
				}
				//printf("5");
		}
		
		// why dont you have to close both sockets
		int shutdown_success_0 = -1;
		shutdown_success_0 = close(fd);
		if (shutdown_success_0 != 0){
				perror("shutdown_0 error");
				exit(1);
		}
		return 0;
}



// TODO:
// handle closed connections with sockets, remove from fd_set and also close() the socket
// 		-- done
// handle diconnection from client
// 		-- done
// handle manual input address, make sure it works with two separate computers
// 1. print address of server
// 2. run client with argv[1] as ip address string
// 3. have the port number predetermined
// 4. test and should work, use mom's mac
// handle closing the clients when the server closes
// 1. find a way to check if the socket is still active/open
// 2. if it's not, exit the while loop and let the program close itself, socket and all
// 1. if you read 'exit' from the server, exit the same way you would for writing it
// comment the code
// 1. go through, explain and clean it up
// 2. clean up the print statements, make it pretty

//incoming message  
/*
   if ((valread = read(sd , buffer, 256)) == 0){   
//Somebody disconnected , get his details and print  
printf("3");
getpeername(sd , (struct sockaddr*)&client_addr , (socklen_t*)&addr_len);
printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(client_addr.sin_addr) , ntohs(client_addr.sin_port));   

//Close the socket and mark as 0 in list for reuse  
close(sd);   
client_socket[i] = 0;   
} 
//Echo back the message that came in  
//set the string terminating NULL byte on the end  
//of the data read  
buffer[valread] = '\0';   
send(sd , buffer , strlen(buffer) , 0 );   
 */  
