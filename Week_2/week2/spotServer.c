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

	PORT_NO = atoi((const char *)argv[1]);

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

	printf("Operation request from client: %s\n", buffer);
	
	int num1, num2;
	char n1[BUF_SIZE], n2[BUF_SIZE];

	int j=0, k=2;
	while(buffer[k] != ' ') {
		n1[j++] = buffer[k++];
	}
	n1[j] = '\0';

	k++;
	j=0;
	while(buffer[k] != '\n' && k != strlen(buffer)) {
		n2[j++] = buffer[k++];
	}
	n2[j] = '\0';

	num1 = atoi((const char *)n1);
	num2 = atoi((const char *)n2);

	float result = 0;
		
	switch(buffer[0]) {
		case '+':
			result = num1 + num2;
			break;

		case '-':
			result = num1 - num2;
			break;

		case '*':
			result = num1 * num2;
			break;

		case '/':
			result = num1 / num2;
			break;
	}


	n = write(newSockFd, &result, sizeof(result));
	if(n < 0) {
		error("Error while writing to socket!");
	}

	printf("Calculation completed!\n");

	return 0;
}
