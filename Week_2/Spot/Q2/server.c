#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
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
	char op;
	int len1 = 0;
	int len2 = 0;
	int i;
	char arr[50];
	printf("Here is the message: %s\n",buffer);
	for(i=0;strlen(buffer) && buffer[i] != '\0';i++){
		if(buffer[i] == ' '){
			op = buffer[i+1];
			i += 3;
			break;
		}
		else{
			len1++;
		}
	}
	len2 = strlen(buffer) - i - 1;
	//printf("Length = %d\nOp = %c\ninde = %d\nlength=%d\n",len1,op,i,len2);
	for(int j=0;j<len1;j++){
		arr[j] = buffer[j];
	}
	arr[len1] = '\0';
//	printf("%s\n",arr);
	int a = atoi(arr);
	char arr2[50];
	int j = 0;
	for(int k = i;k<strlen(buffer);k++){
		arr2[j] = buffer[k];
		j++;
	}
	arr2[len2] = '\0';
//	printf("%s\n",arr2);
	int b = atoi((const char*)arr2);
//	printf("%d %d\n",a,b);
	float res;
	switch(op){
		case '+':
			res = a + b;
			break;
		case '-':
			res = a - b;
			break;
		case '*':
			res = a * b;
			break;
		case '/':
			res = (float)a / b;
			break;
		default:
			printf("Invalid\n");
			break;
	}
	printf("float: %f\n",res);	
	n = write(new_socket_fd, &res, sizeof(res));
	if(n < 0){
		error("Error WRITING TO SOCKET\n");
	}
	return 0;
}
