#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void error(char * msg){
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]){
	int socket_fd, port_no, n;
	struct sockaddr_in server_addr;
	struct hostent *server;
	char buffer[256];
	if(argc < 3){
		fprintf(stderr, "Usage %s Hostname Port\n",argv[0]);
		exit(0);
	}
	port_no = atoi(argv[2]);
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		error("Error OPENING SOCKET\n");
	}
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "Error NO SUCH HOST\n");
		exit(0);
	}
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_port = htons(port_no);
	if(connect(socket_fd, &server_addr, sizeof(server_addr)) < 0){
		error("Error CONNECTING");
	}
	printf("Please enter the message\n");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	n = write(socket_fd, buffer, strlen(buffer));
	if(n < 0){
		error("Error WRITING TO SOCKET");
	}
	bzero(buffer, 256);
	n = read(socket_fd, buffer, 255);
	if(n < 0){
		error("Error READING FROM SOCKET");
	}
	printf("%s\n",buffer);
	return 0;
}

