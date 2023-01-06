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
		printf("Port not provided\n");
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

	servaddr.sin_family    = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(atoi(argv[1])); 

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE);
	} 

	int len, n; 
	len = sizeof(cliaddr);  

	n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
	buffer[n] = '\0'; 
	printf("From Client Received: %s\n", buffer); 
	printf("Enter your message for the client\n");
	fgets(hello,50,stdin);
	sendto(sockfd, (char *)hello, strlen(hello), MSG_CONFIRM, (struct sockaddr *) &cliaddr, len); 
	printf("Message sent.\n");  

	return 0; 
}
