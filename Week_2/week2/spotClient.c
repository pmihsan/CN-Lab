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
		error("No port specified!\n");
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

	char op[BUF_SIZE], num[BUF_SIZE];
	
	printf("Enter the operation(+ or - or / or *): ");
	bzero(op, BUF_SIZE);
	scanf("%s", op);
	strcat(op, " ");
	getchar();

	printf("Enter two numbers: ");
	fgets(num, BUF_SIZE, stdin);

	strcat(op, num);
	printf("%s",op);

	n = write(sockFd, op, strlen(op));
	if(n < 0) {
		error("Error while sending input!\n");
	}

	
	if(n < 0) {
		error("Error while writing to socket!");
	} else {
		printf("Message sent successfully!\n");
		float ans;
		n = read(sockFd, &ans, sizeof(float));
		printf("Server response: %f\n", ans);
	}

	return 0;
}

