#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#define MAXLINE 1024 

int main(int argc, char *argv[]){
	if(argc <= 1){
		printf("No Port Provided\n");
		return 0;
	}
	int sockfd; 
	char buffer[MAXLINE]; 
	char hello[50]; 
	struct sockaddr_in servaddr, cliaddr; 

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(atoi(argv[1])); 

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE);
	} 
	
	printf("Server started running...\n");

	while(1) {

		int len, n; 
		len = sizeof(cliaddr);  

		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
		buffer[n] = '\0'; 
		printf("Connection accepted from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		printf("From Client Received: %s",buffer); 
		//printf("Enter message for client\n");
		//fgets(hello,50,stdin);
		strcpy(hello,"Response Message Sent");
		sendto(sockfd, (char *)hello, strlen(hello), MSG_CONFIRM, (struct sockaddr *) &cliaddr, len); 
		printf("Reply message sent to a client.\n");  
	}

	return 0; 
}
