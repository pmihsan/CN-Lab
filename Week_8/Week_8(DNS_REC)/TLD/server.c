#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../common.h"
extern int errno;
int main() {
        int socketfd = 0, authfd = 0, sentbytes, recvbytes;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, client_addr, auth_addr;
        char buffer[64],req_ip[30];

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(TLDPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }
        
        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);

                fprintf(stdout, "Request received for : %s.\n", buffer);
		char domain[200];
		int i=0,j=0;
		while(buffer[i++] != '.');
		while(buffer[i] != '.'){
			domain[j++] = buffer[i++];
		}
		domain[j++] = buffer[i++];
		while(buffer[i] != '\0')
			domain[j++] = buffer[i++];
		domain[j] = '\0';
		fprintf(stdout,"Domain: %s",domain);
                FILE* fd = fopen("tlddns.txt", "r");
                if(!fd) {
                        fprintf(stderr, "Could not access DNS records.\n");
                        sendto(socketfd, "ERROR", strlen("ERROR") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }
                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';
                while( fgets(linebuff, sizeof(linebuff), fd) ) {
                        strcpy(tempbuff, linebuff);
                        temp = strtok(tempbuff, " ");
                        if(flag == 0 && strncmp(temp,domain, strlen(domain)) == 0) {
                                flag = 1;
                                strcpy(lastbuff, linebuff);
                                iptemp = strtok(NULL, "\n");
                                for(i = 0; *iptemp != '\0'; i++, iptemp++)
                                        ip[i] = *iptemp;
                                ip[i] = '\0';
                        }
                        else {
                                strcat(filebuff, linebuff);
                        }
                }
                fclose(fd);
                if(flag == 0) {
                        sentbytes = sendto(socketfd, "404", strlen("404") + 1, 0, (struct sockaddr*)&client_addr, length);
                	continue;
		}
                else {
                        int fdes = open("tlddns.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "Response : %s.\n\n", ip);
                }
	        
		// AUTH DNS
		authfd = socket(AF_INET,SOCK_DGRAM,0);
		if(authfd < 0){
			fprintf(stderr,"Error in auth socket sreation\n");
			return -1;
		}
	
		auth_addr.sin_family = AF_INET;
		auth_addr.sin_port = htons(AUTHPORT);
		inet_pton(AF_INET, "127.0.0.1", &auth_addr.sin_addr);

		sentbytes = sendto(authfd, buffer,strlen(buffer)+1,0,(struct sockaddr*)&auth_addr,sizeof(auth_addr));
		recvbytes = recvfrom(authfd,req_ip,sizeof(req_ip),0,NULL,NULL);
		fprintf(stdout,"Resonse from Auth for %s: %s\n",buffer,req_ip);		

                sentbytes = sendto(socketfd, req_ip, strlen(req_ip) + 1, 0, (struct sockaddr*)&client_addr, length);

        }
        close(socketfd);
        return 0;
}
