#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT 8080 
#define MAXLINE 1024 

int main(int argc, char *argv[]) { 
	if(argc <= 1){
		printf("No Port Provided\n");
		return 0;
	}
	int sockfd; 
	char buffer[MAXLINE]; 
	char hello[50]; 
	struct sockaddr_in servaddr; 

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(atoi(argv[1])); 
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int n, len; 
	printf("Enter your message for the server\n");
	fgets(hello,50,stdin);
	sendto(sockfd, (char *)hello, strlen(hello), MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr)); 
	printf("Message sent.\n"); 

	n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
	buffer[n] = '\0'; 
	printf("From Server Received: %s\n", buffer); 

	close(sockfd); 
	return 0; 
}
