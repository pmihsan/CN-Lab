#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVERPORT 5047
#define PROXYPORT 5048

void handleConnection(int clientfd) {
        FILE *log_file = NULL,*fd = NULL;
        char request[512], filename[20], timestamp[30], response[512], buff[512], line[100];
        char filebuffer[512], linebuffer[512], logwrite[512], temp[512];
        char server_response[512], *filebuff;
        int socketfd = 0;
        struct sockaddr_in server_addr;
        int i = 0;
        response[0] = '\0';
        request[0] = '\0';
        filebuffer[0] = '\0';
        recv(clientfd, request, sizeof(request), 0);
        while(request[i] != '\0' && request[i] != '\n') {
                filename[i] = request[i];
                i++;
        }
        filename[i] = '\0';
        
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                strcpy(response, "error");
                send(clientfd, response, strlen(response) + 1, 0);
                return;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVERPORT);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if(connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                fprintf(stderr, "Error in connecting to server.\n");
                strcpy(response, "error");
                send(clientfd, response, strlen(response) + 1, 0);
                return;
        }
        fprintf(stdout, "Connected to server.\n");
        
        request[0] = '\0';
        strcat(request, filename);
        strcat(request, "$");
        log_file = fopen(filename, "r");
        if(log_file == NULL) {
                fprintf(stdout, "Student record not found in cache.\n");
                send(socketfd, request, strlen(request) + 1, 0);
                recv(socketfd, filebuffer, sizeof(filebuffer), 0);  
        	if(strncmp(filebuffer, "404", 3) != 0) {
			fd = fopen(filename, "w");
        		fputs(filebuffer, fd);
        		fclose(fd);
		}
	} else {
		fprintf(stdout, "Student record found in cache.\n");
		fd = fopen(filename, "r");	
		while(fgets(line, sizeof(line), fd)) {
			strcat(filebuffer, line);
		}
		if(filebuffer[strlen(filebuffer)-1] != '\n') {
			strcat(filebuffer, "\n");
		}
		fclose(fd);
	}
	
	send(clientfd, filebuffer, sizeof(filebuffer), 0);
	close(socketfd);
	fprintf(stdout, "Server connection closed.\n");
        return;
}
int main() {
        int socketfd = 0, clientfd = 0, length = 0;
        struct sockaddr_in host_addr, client_addr;
        char buffer[1024];

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(PROXYPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port.\n");
                return -1;
        }

        if(listen(socketfd, 5) < 0) {
                fprintf(stderr, "Error in listening on port.\n");
                return -1;
        }
        fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

        while(1) {
                clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
                if(clientfd < 0) {
                        fprintf(stderr, "Error in accepting connection.\n");
                        return -1;
                }
                fprintf(stdout, "New Connection established.\n");
                handleConnection(clientfd);
                close(clientfd);
		fprintf(stdout, "Client connection closed.\n");
        }
        close(socketfd);
        return 0;
}
