#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>

void error(char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	int socket_fd, new_socket_fd, port_no, client_addr_len, n;
	char buffer[256];
	struct sockaddr_in server_addr, client_addr;	
	if(argc < 2){
		fprintf(stderr, "Error NO PORT PROVIDED\n");
		exit(1);
	}
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		error("Error OPENING SOCKET\n");
	}
	bzero((char*) &server_addr, sizeof(server_addr));
	port_no = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(socket_fd, (struct sockarr *) &server_addr, sizeof(server_addr)) < 0){
		error("Error ON BINDING\n");
	}
	listen(socket_fd, 5);
	client_addr_len = sizeof(client_addr);
	new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);
	if(new_socket_fd < 0){
		error("Error ON ACCEPT\n");
	}
	bzero(buffer, 256);
	n = read(new_socket_fd, buffer, 255);
	if(n < 0){
		error("Error READING FROM SOCKET\n");
	}
	printf("Here is the message: %s\n",buffer);
	for(int i=0;i<strlen(buffer) &&buffer[i] != '\0';i++){
		if(buffer[i] >= 97 && buffer[i] <= 122){
			buffer[i] = (char)buffer[i] - 32;
		}
	}
	//printf("Here is the message: %s\n",buffer);
	n = write(new_socket_fd, buffer, 256);
	if(n < 0){
		error("Error WRITING TO SOCKET\n");
	}
	return 0;
}
