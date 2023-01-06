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

#define SERVERPORT 1502

typedef struct
{
    char filename[32];
    char username[32];
    char password[32];
} request;

typedef struct
{
    int status;
    char msg[32];
    long PORTNO;
} response;

void printReq(request *r)
{
    printf("\nRequest for: %s\nUsername: %s\nPassword: %s\n", r->filename, r->username, r->password);
}

void printRes(response *r)
{
    printf("\nStatus: %d\n Message: %s\n", r->status, r->msg);
    if (r->status == 200)
        printf("Port No.: %ld\n", r->PORTNO);
}

int get_connected_socket(long port)
{
    int socketfd = 0, n = 0;
    struct sockaddr_in server_addr;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        fprintf(stderr, "Error in socket creation.\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "Error in connection.\n");
        return -1;
    }
    fprintf(stdout, "Connection established with %s:%d.\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    return socketfd;
}

void file_transfer(long PORTNO)
{
    int socketfd = 0;
    char filename[32], buffer[512];
    filename[0] = '\0';
    response res;
    char cnfm;

    socketfd = get_connected_socket(PORTNO);
    if (socketfd < 0)
    {
        return;
    }

    fprintf(stdout, "Enter exit to close the connection.\n");
    while (1)
    {
        printf(">> Enter required filename: ");
        scanf("%s", filename);
        printf(">> Sending to server@%d\n", PORTNO);
        if (strncmp(filename, "exit", strlen("exit")) == 0)
        {
            send(socketfd, "exit", strlen("exit") + 1, 0);
            break;
        }
        send(socketfd, filename, strlen(filename) + 1, 0);
        recv(socketfd, &res, sizeof(res), 0);
        printf("<< receiving from server@%d\n", PORTNO);
        printRes(&res);
    }
    close(socketfd);
    return;
}

int main()
{
    int socketfd;
    struct sockaddr_in server_addr;

    socketfd = get_connected_socket(SERVERPORT);
    request req;
    response res;
    char username[32], password[32];

CRED:
    bzero(username, sizeof(username));
    bzero(password, sizeof(password));
    printf(">> Enter Username: ");
    scanf("%s", username);
    printf(">> Enter password: ");
    scanf("%s", password);
    strcpy(req.username, username);
    strcpy(req.password, password);
    strcpy(req.filename, "req_handshake");

    printf(">> Sending to server@%d\n", SERVERPORT);
    printReq(&req);
    send(socketfd, &req, sizeof(req), 0);

    recv(socketfd, &res, sizeof(res), 0);
    printf("<< received from server@%d\n", SERVERPORT);
    printRes(&res);
    if (res.status == 200)
    {
        file_transfer(res.PORTNO);
    }
    else
    {
        printf("Error. Try again.\n\n");
        goto CRED;
    }

    close(socketfd);
    return 0;
}