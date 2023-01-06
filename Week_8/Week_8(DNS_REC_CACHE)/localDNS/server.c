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

int main() {
        int socketfd = 0, localfd = 0;
        int rootfd = 0, tldfd = 0, authfd = 0;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, root_addr, tld_addr, auth_addr, client_addr;
        char buffer[512], buf[512],root[20], tld[30], auth[100];
        char req_ip[30], tldip[30], authip[30];
        int recvbytes, sentbytes;

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(LOCALPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }

        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);
                if(strncmp(buffer, "exit", sizeof("exit")) == 0)
                        break;
                fprintf(stdout, "Request from client : %s\n", buffer);
		
		
                FILE* fd = fopen("cache.txt", "r");
                if(!fd) {
                        fprintf(stderr, "Could not access CACHE records.\n");
                        continue;
                }
                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0, i;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';
                while( fgets(linebuff, sizeof(linebuff), fd) ) {
                        strcpy(tempbuff, linebuff);
                        temp = strtok(tempbuff, " ");
                        if(flag == 0 && strncmp(temp, buffer, strlen(temp)) == 0) {
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
                
		if(flag == 1) {
			printf("Cache HIT...\n");
                	sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
			continue;
		}	
		printf("Cache MISS...\n");
                //ROOTDNS
                rootfd = socket(AF_INET, SOCK_DGRAM, 0);
                if(rootfd < 0) {
                        fprintf(stderr, "Error in socket creation.\n");
                        return -1;
                }      

                root_addr.sin_family = AF_INET;
                root_addr.sin_port = htons(ROOTPORT);
                inet_pton(AF_INET, "127.0.0.1", &root_addr.sin_addr);  
                
                sentbytes = sendto(rootfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&root_addr, sizeof(root_addr));
                recvbytes = recvfrom(rootfd, req_ip, sizeof(req_ip), 0, NULL, NULL);
                fprintf(stdout, "Server IP for %s: %s.\n", buffer, req_ip);
                close(rootfd);
		
		printf("Updating Cache...\n");
		fd = fopen("cache.txt","a");
		fprintf(fd, "%s %s\n",buffer,req_ip);
		fclose(fd);

                sentbytes = sendto(socketfd, req_ip, strlen(req_ip) + 1, 0, (struct sockaddr*)&client_addr, length);
        }
        close(socketfd);
        return 0;
}       
