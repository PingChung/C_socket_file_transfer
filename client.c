#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define DST_PORT 45566
#define MAX_BUFF 65535
#define IP_LENGTH 16

int main()
{
	int client_fd;
	struct sockaddr_in client_content;
	char read_buf[MAX_BUFF];
	char write_buf[MAX_BUFF];
	int read_num;
	FILE *file_fd;
	char server_ip[IP_LENGTH];
	while(1)
	{
		printf("Server IP: ");
		fflush(stdout);		
		scanf("%s", server_ip);
		if(strlen(server_ip) > 15)
		{
			printf("Buffer Overflow. Try again~\n");
			fflush(stdout);	
		}
		else
			break;	
	}	
	printf("Server IP is : %s\n", server_ip);	

	//initial socket
	if ( (client_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket error.\n");
		exit(1);
	}

	//initial sockaddr
	bzero(&client_content, sizeof(client_content));
	client_content.sin_family = AF_INET;
	client_content.sin_port = htons(DST_PORT);
	client_content.sin_addr.s_addr = inet_addr(server_ip);

	//connect()	
	if ((connect(client_fd, (struct sockaddr*)&client_content, sizeof(client_content))) == -1)
	{
		perror("Connect error.\n");
		exit(1);
	}

	while (1)
	{
		printf("\n>> ");
		fflush(stdout);
		
		scanf("%s", write_buf);
		if ((send(client_fd, write_buf, strlen(write_buf), 0)) == -1)
		{
			printf("Warning , send command to server fail.\n");
			continue;
		}
		
		//open a file
		file_fd = fopen(write_buf, "wb");
		if(file_fd == NULL)
		{
			printf("Error opening file");
			return 1;
		}
		
		while ((read_num = recv(client_fd, read_buf, sizeof(read_buf), 0)) > 0)
		{
			printf("%s", read_buf);
			fwrite(read_buf, sizeof(char), read_num, file_fd);
			bzero(&read_buf,sizeof(read_buf));
			if (read_num<sizeof(read_buf))
				break;
		}
		fclose(file_fd);
	}
	

	close(client_fd);
	return 0;
}
