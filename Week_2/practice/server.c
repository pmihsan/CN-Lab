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
	struct sockaddr_in server,client;
	
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
	//INET_ADDR("127.0.0.1")
	int x=bind(sockfd, (struct sockaddr *)&server, sizeof(server));
	if(x==-1){
		printf("bind Failure\n");
		exit(0);
	}else{
		printf("bind Successful\n");
	}
	
	listen(sockfd, 5);
	int s=sizeof(client);
	
	struct myStruct st[4]={{"pm1",1,"pm1"},
						   {"pm2",2,"pm2"},
						   {"pm3",3,"pm3"},
						   {"pm4",4,"pm4"}};
	struct myStruct myst;
	int fd1=accept(sockfd, (struct sockaddr *)&client, &s);
	
	recv(fd1, &myst, sizeof(myst), 0);
	//recvfrom(sockfd,buff,100,0,(struct sockaddr *)&client,&s);
	
	printf("userId received :%d\n",myst.userId);
	printf("password received :%s\n",myst.password);
	int fl=1;
	for(int i=0;i<4;i++){
		if(myst.userId==st[i].userId && strcmp(myst.password,st[i].password)==0){
			fl=0;
			break;
		}
	}
	
	
	if(fl==0){		
		char message[20]="Login Successful\n";
		send(fd1, message, sizeof(message), 0);
	}else{
		char message[20]="Login Failure\n";
		send(fd1, message, sizeof(message), 0);
	}	
	
	
	close(sockfd);
	return 0;
}
