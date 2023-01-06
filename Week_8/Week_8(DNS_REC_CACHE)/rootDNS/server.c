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
        int socketfd = 0, tldfd = 0, sentbytes, recvbytes;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, tld_addr, client_addr;
        char buffer[64];
	char req_ip[30];
	int r_bytes, s_bytes;

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(ROOTPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }
        
        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);

                fprintf(stdout, "Request received for : %s.\n", buffer);
		char domain[6];
		int i=0,j=0;
		while(buffer[i++] != '.');
		while(buffer[i++] != '.') ;
		
		while(buffer[i] != '\0')
			domain[j++] = buffer[i++];
		domain[j] = '\0';
	
                FILE* fd = fopen("rootdns.txt", "r");
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
                        if(flag == 0 && strncmp(temp, domain, strlen(temp)) == 0) {
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
                        int fdes = open("rootdns.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "Response : %s.\n\n", ip);
                //        sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
                }


                fd = fopen("cache.txt", "r");
                if(!fd) {
                        fprintf(stderr, "Could not access CACHE records.\n");
                        continue;
                }
                //char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                //char *temp, *iptemp;
                //int flag = 0, i;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';
		flag = 0;
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


		// TLD DNS
		tldfd = socket(AF_INET,SOCK_DGRAM,0);
		if(tldfd < 0){
			fprintf(stderr,"Error in tld socket creation\n");
			return -1;
		}
        	tld_addr.sin_family = AF_INET;
        	tld_addr.sin_port = htons(TLDPORT);
        	inet_pton(AF_INET, "127.0.0.1", &tld_addr.sin_addr);
		
		s_bytes = sendto(tldfd,buffer,strlen(buffer)+1,0,(struct sockaddr*)&tld_addr,sizeof(tld_addr));
		r_bytes = recvfrom(tldfd,req_ip,sizeof(req_ip)+1,0,NULL,NULL);
		fprintf(stdout,"Resonse from TLD for %s: %s\n",buffer,req_ip);
		close(tldfd);
		
		fd = fopen("cache.txt","a");
		fprintf(fd,"%s %s\n",buffer,req_ip);
		fclose(fd);
	
		s_bytes = sendto(socketfd,req_ip,strlen(req_ip)+1,0, (struct sockaddr*)&client_addr, length);

        }
        close(socketfd);
        return 0;
}
