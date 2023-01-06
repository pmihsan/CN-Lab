#include "../common.h"
#include "../commHeader.h"

int main() {
	int s_fd = 0, s_bytes, r_bytes;
	struct sockaddr_in local_host_addr;
	char in[50], buf[50];

	s_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(s_fd < 0){
		printf("[-]Error creating socket\n");
		return 1;
	}
	
	local_host_addr.sin_family = AF_INET;
	local_host_addr.sin_port = htons(LOCALDNS);
	inet_pton(AF_INET, "127.0.0.1", &local_host_addr.sin_addr);
	while(1){
		printf("[*]Enter the hostname\n>>> ");
		scanf("%s",in);
                s_bytes = sendto(s_fd, in, strlen(in) + 1, 0, (struct sockaddr*)&local_host_addr, sizeof(local_host_addr));
                if(strncmp(in, "exit", sizeof("exit")) == 0){
                        printf("[-]Exiting...\n");
			break;
		}
                r_bytes = recvfrom(s_fd, buf, sizeof(buf), 0, NULL, NULL);
                printf("[+]SERVER IP OF %s : %s\n", in, buf);
        }
        
        close(s_fd);
        return 0;
}
