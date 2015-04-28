#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define BIND_PORT 45566
#define MAX_BUFF 65535

int main()
{
	int server_fd;
	int client_fd;
	socklen_t client_size;
	struct sockaddr_in server_content;
	struct sockaddr_in client_content;
	int read_num = 0;
	char read_buf[MAX_BUFF];
	char write_buf[MAX_BUFF];
	FILE *fp;
	int n_bytes = 0;
	char *warning_string = "Cannot open file , try another one.\n";
	
	// socket initial: socket()
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket error.\n");
		exit(1);
	}

	// initial sockaddr
	bzero(&server_content, sizeof(server_content));
	server_content.sin_family=AF_INET;
	server_content.sin_port=htons(BIND_PORT);
	server_content.sin_addr.s_addr=INADDR_ANY;

	// bind()
	if (bind(server_fd, (struct sockaddr*)&server_content, sizeof(server_content)) != 0){
		perror("bind error.\n");
		exit(1);
	}
	
	// listen()
	if ((listen(server_fd,5)) != 0){
		perror("listen error.\n");
		exit(1);
	}

	while(1){		
		// initial client information
		bzero(&client_content, sizeof(client_content));
		client_size = sizeof(client_content);
		client_fd = -1;

		printf("Wait for client command.\n");
		fflush(stdout);		
		
		// accept connection from client: accept()
		client_fd = accept(server_fd, (struct sockaddr*)&client_content, &client_size);
		if (client_fd == -1){
			printf("accept connection fail , try again.\n");
			continue;
		}
				
		// receive command from client and reutrn the data: recv()
		while (1){
			memset(read_buf, '\0', sizeof(read_buf));
			n_bytes = recv(client_fd, read_buf, sizeof(read_buf), 0);
			
			if (n_bytes == 0 || NULL) //connection close
				break;
			if (read_buf[strlen(read_buf)-1]=='\n')
				read_buf[strlen(read_buf)-1]='\0';
			if (read_buf[0]=='\0')
				continue;

			printf("Open %s\n", read_buf);
			fflush(stdout);

			// open file error
			fp = fopen(read_buf, "rb");
			if (fp == NULL){
				printf("Open %s fail.\n", read_buf);
				fflush(stdout);
				strncpy(write_buf, warning_string, strlen(warning_string));

				if ((send(client_fd, write_buf, strlen(write_buf), 0)) == -1)
				{
					printf("Write to client fail.\n");
					fflush(stdout);
				}	

				continue;
			}

			// send file
			while((n_bytes = fread(write_buf, sizeof(char), sizeof(write_buf), fp)) > 0){			
			//while(fp != NULL){
			//while (read(file_fp, write_buf, sizeof(write_buf)) > 0){
				//n_bytes = fread(write_buf, sizeof(char), sizeof(write_buf), fp);
				send(client_fd, write_buf, n_bytes, 0);
				bzero(&write_buf, sizeof(write_buf));
			}

			printf("Write to client end.\n");
			fflush(stdout);
			fclose(fp);
		}

	}
	
	// close socket: close()
	close(client_fd);
	close(server_fd);
	
	return 0;

}
