#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define BUF_SIZE 256

int PORT_NO;

void error(char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	
	if(argc <= 1) {
		error("No port given!\n");
	}
	PORT_NO = atoi((const char *) argv[1]);

	int sockFd, newSockFd, cliLen, n;
	struct sockaddr_in server_addr, client_addr;
	char buffer[BUF_SIZE];

	// creating a socket: Internet (address domain), stream (socket type) and default protocol
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0) {
		error("Socket creation failed!\n");
	}
	
	// initializing socket address
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NO);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// binding created socket to socket address
	if( bind( sockFd, (struct sockaddr *) &server_addr, sizeof(server_addr) ) < 0) {
		error("Socket binding failed!\n");
	}

	// listening
	if(listen(sockFd, 3) < 0) {
		error("Listen failed!\n");
	}
	
	printf("Listening on PORT: %d\n", PORT_NO);

	// waiting for client connection
	cliLen = sizeof(client_addr);
	newSockFd = accept(sockFd, (struct sockaddr *) &client_addr, &cliLen);
	if(newSockFd < 0) {
		error("ERROR on accept!");
	}

	// after establishing connection
	bzero(buffer, BUF_SIZE);
	n = read(newSockFd, buffer, BUF_SIZE);
	if(n < 0) {
		error("Error while reading from socket!");
	}

	printf("Lower case string from client: %s\n", buffer);
	char newStr[BUF_SIZE];
	int j;
	for(j=0; j<strlen(buffer); j++) {
		newStr[j] = toupper(buffer[j]);
	}
	newStr[j] = '\0';

	n = write(newSockFd, newStr, strlen(newStr) + 1);
	if(n < 0) {
		error("Error while writing to socket!");
	}

	return 0;
}
