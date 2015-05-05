#include <stdlib.h>        // for exit
#include <stdio.h>        // for printf
#include <string.h>        // for bzero
#include <sys/socket.h>    // for socket
#include <sys/types.h>    // for socket
#include <netinet/in.h>    // for sockaddr_in
#include <arpa/inet.h>  //for inet_addr
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define HELLO_WORLD_SERVER_PORT    6666
#define PORT 45566
#define BUFFER_SIZE 65535
#define FILE_NAME_LENGTH 65535

int main(int argc, char **argv)
{
    int client_socket;
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Client Socket Failed!\n");
        exit(1);
    }
    
    /* Optional
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);
    
    if(bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Bind Failed!\n");
        exit(1);
    }
    printf("Bind Success\n");
    fflush(stdout);
    Optional */
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    
    if((connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0){
        printf("Connect Failed\n");
        exit(1);
    }

    char file_name[FILE_NAME_LENGTH + 1];
    bzero(file_name, FILE_NAME_LENGTH + 1);
    printf(">> ");
    scanf("%s", file_name);

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
    
    if((send(client_socket, buffer, BUFFER_SIZE, 0)) == -1){
        printf("Send Failed\n");
        exit(1);
    }

    FILE *fp = fopen(file_name, "wb");
    if(fp == NULL){
        printf("Open File Failed\n");
        exit(1);
    }
    else{
        bzero(buffer, BUFFER_SIZE);
        int length = 0;

        while((length = recv(client_socket, buffer, BUFFER_SIZE, 0))){
            if(length < 0){
                printf("Recieve Failed!\n");
                break;
            }

            int write_length = fwrite(buffer, sizeof(char), length, fp);
            if (write_length < length){
                printf("Write Failed\n");
                break;
            }

            bzero(buffer,BUFFER_SIZE);   
        }

        printf("Recieve File Finished\n");
        fclose(fp);
    }
    
    close(client_socket);
    return 0;
}