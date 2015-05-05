#include <stdlib.h>     // for exit
#include <stdio.h>      // for printf
#include <string.h>     // for bzero
#include <sys/socket.h> // for socket
#include <sys/types.h>  // for socket
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>  //for inet_addr
#include <fcntl.h>      // for open
#include <unistd.h>     // for close

#define PORT 45566
#define LENGTH_OF_LISTEN_QUEUE 1
#define BUFFER_SIZE 65535
#define FILE_NAME_LENGTH 65535

int main()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int server_socket;
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Create Socket Failed!");
        exit(1);
    }

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        printf("Bind Failed!");
        exit(1);
    }

    if(listen(server_socket, LENGTH_OF_LISTEN_QUEUE)){
        printf("Server Listen Failed!");
        exit(1);
    } 
    
    while (1){
        printf("Server is Listening......\n");
        fflush(stdout);

        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if (client_socket < 0){
            printf("Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        
        int recv_length = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_length < 0)
        {
            printf("Recieve Command Failed!\n");
            break;
        }

        char file_name[FILE_NAME_LENGTH+1];
        bzero(file_name, FILE_NAME_LENGTH+1);
        strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_LENGTH ? FILE_NAME_LENGTH : strlen(buffer));

        FILE * fp = fopen(file_name,"rb");
        if(fp == NULL){
            printf("File Not Found\n");
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int read_length = 0;

            while((read_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){

                if(send(client_socket, buffer, read_length, 0) < 0){
                    printf("Send File Failed\n");
                    break;
                }
                bzero(buffer, BUFFER_SIZE);
            }

            fclose(fp);
            printf("File Transfer Finished\n\n");
        }

        close(client_socket);

    }
    

    close(server_socket);
    return 0;
}
