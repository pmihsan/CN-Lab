#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define domain AF_INET
#define type SOCK_STREAM
#define protocol 0
#define port 8097
#define backlog 10
#define sz 1000

struct header{
char filename[sz];		        
char method[sz];
 int length;
      time_t last_modified;
        time_t res_time;
       char msg[sz];
};
void http_persistent(int client)
{
            struct header x,y;
               recv(client,(struct header*)&y, sizeof(struct header),0);
                if(strcmp(y.method,"exit")==0)
                {															                        exit(1);																                }
              printf("The file name is %s\n", y.filename);
               printf("The method is %s\n", y.method);
               if(strcmp(y.method,"GET")==0)
															                {
														                        FILE *fp=fopen(y.filename,"r");
														                        strcpy(x.method,"GET");
														                        time(&x.res_time);
														                        if(fp==NULL)
														                        {
             x.length=0;
             strcpy(x.msg,"404 Not found - The file doesnot exit!!");
																														                        }
                 else
                    {
															                                time(&x.last_modified);
																														                                x.length=fscanf(fp,"%[^EOF]", &x.msg);
																														                        }
        }
         else if(strcmp(y.method,"DELETE")==0)
																				                {
															                        FILE *fp=fopen(y.filename,"r");
															                        if(fp==NULL)
															                        {
             strcpy(x.msg,"404 Not found - The file doesnot exit!!");
														                        }
														                        else
														                        {
																	      if(remove(y.filename)) strcpy(x.msg,"The file not deleted");
																														                                else strcpy(x.msg,"The file deleted sucessfully");
																														                        }
																															                }
																	  else
																	          {
																	             time(&x.res_time);
																	                 x.length=0;
																	                  strcpy(x.msg,"400 Bad Request");
																	          }
																	             send(client,(void*)&x,sizeof(struct header),0);
	
}
void test(int client)
{
	        char a[sz];
		        recv(client,&a,sizeof(a),0);
			        bzero(a, sz);
				        strcpy(a,"connection is good");
					        send(client,&a,sz,0);
}
int main()
{
		while(1){
	        int server;
		        server=socket(domain,type,0);
			        if(server==-1)
			        {
		                printf("Error is creating socket at server side \n");
		                exit(0);
			        }
			        else
			        {
		                printf("Server Socket created sucessfully\n");
			        }
			        struct sockaddr_in serv_addr,cli;
			        serv_addr.sin_family=domain;
			        serv_addr.sin_port=htons(port);
			        serv_addr.sin_addr.s_addr=INADDR_ANY;
			       if(bind(server,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
			        {
			         printf("Error in binding the port with socket \n");
			         close(server);
			         exit(0);
																			        }
		        else
		        {
		            printf("Binded to the port successfully\n");
		        }
		        if(listen(server,backlog)==-1)
		        {
		         printf("Error in listening to client\n");
		          close(server);
																		                exit(0);
																			        }
		        else
		      {
		           printf("Listening to the client :)...\n");
		       }
		       char msg[sz],buff[sz];
		       strcpy(msg,"Connected to server sucessully");
		       int len=sizeof(cli);
		      int client=accept(server,(struct sockaddr*)&cli, &len);
																	        if(client==-1)
																			        {
																                printf("Error in accepting the resquest\n");
																                close(server);
																                exit(0);
																	        }
		    else
																	        {
																                printf("Acceptedthe request successfully\n");
																	        }
																	        http_persistent(client);
																	        close(server);
	printf("Connection Closed\n");	
}													      
return 0;
}
