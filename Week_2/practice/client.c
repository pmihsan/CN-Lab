#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
struct myStruct{
	char name[10];
	int userId;
	char password[10];
};
int main(){
	int sockfd;
	struct sockaddr_in server;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1){
		printf("Socket not created\n");
		exit(0);
	}else{
		printf("Socket Created\n");
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(3021);
	server.sin_addr.s_addr = INADDR_ANY;
	
	
	int x=connect(sockfd, (struct sockaddr *)&server, sizeof(server));
	if(x==-1){
		printf("Connection Failure\n");
		exit(0);
	}
	
	struct myStruct myst;
	printf("Enter userId : "); 
	scanf("%d",&myst.userId);
	printf("Enter password : "); 
	scanf("%s",myst.password);

	
	send(sockfd,&myst,sizeof(myst),0);
	
	char message[20];
	recv(sockfd, message, sizeof(message), 0);
	
	printf("%s",message);
	
	
	close(sockfd);
	return 0;
}
