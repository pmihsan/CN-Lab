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

int main() { 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *clientMsg; 
	
	clientMsg = (char *)malloc(sizeof(char) * MAXLINE);

	struct sockaddr_in servaddr; 

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
	while(1) {
		int menu;
		printf("Enter 1 to send message, 2 to exit: ");
		scanf("%d", &menu);
		
		if(menu == 2) {
			printf("Exiting...\n");
			break;
		}
		
		getchar();
		printf("Enter the message: ");
		fgets(clientMsg, MAXLINE, stdin);

		int n, len; 

		sendto(sockfd, (const char *)clientMsg, strlen(clientMsg), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
		printf("Message sent.\n"); 

		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
		buffer[n] = '\0'; 
		printf("Server : %s\n", buffer); 

	}
	close(sockfd); 
	return 0; 
}
