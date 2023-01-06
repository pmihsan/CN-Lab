#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]){

	if(argc == 1){
		printf("Enter a port number\n");
		return 1;
	}
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	int PORT = atoi(argv[1]);
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Connected to Server.\n");

	while(1){
		printf("Enter message for the server: \t");
		//scanf("%s", &buffer);
		bzero(buffer, sizeof(buffer));
		fgets(buffer,1024,stdin);
		write(clientSocket, buffer, 1024);
		//write(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("Disconnected from server.\n");
			exit(1);
		}

		if(read(clientSocket, buffer, 1024) < 0){
			printf("Error in receiving data.\n");
		}else{
			
			printf("Server responds:\t%s\n", buffer);
		}
	}
	return 0;
}
