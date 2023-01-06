#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#define BUF_SIZE 1025

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

	int sockFd, n;
	struct sockaddr_in server_addr;
	struct hostent *server;
	
	char buffer[BUF_SIZE];

	// creating a socket
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFd < 0) {
		error("Socket creation failed!");
	}

	server = gethostbyname("localhost");
	if(server == NULL) {
		error("Server error!\n");
	}

	// initalizing socket address
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_port = htons(PORT_NO);

	// connection
	if(connect(sockFd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		error("Error while connecting!");
	}

	// after connection
	printf("Connection success!\n");

	printf("Enter a lower case string: ");
	bzero(buffer, BUF_SIZE);
	fgets(buffer, BUF_SIZE, stdin);
	n = write(sockFd, buffer, strlen(buffer));
	
	if(n < 0) {
		error("Error while writing to socket!");
	} else {
		printf("Message sent successfully!\n");
		n = read(sockFd, buffer, BUF_SIZE);
		printf("Server response: %s\n", buffer);
	}


	return 0;
}

