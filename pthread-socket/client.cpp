#include "config.h"

void* recv_message(void* pfd)
{
	int sockfd = *(int *)pfd;
	char buff[MAX_LINE];
	ssize_t bufflen;
	while(1)
	{
		if((bufflen = recv(sockfd, buff, MAX_LINE, 0)) < 0 )
		{
			perror("recv error");
			exit(1);
		}
		if(bufflen == 0)
		{
			printf("Server is down.\n");
			exit(0);
		}
		printf("Server:%s", buff);
		if(strcmp(buff, "byebye\n") == 0)
		{
			printf("Server is exit.\n");
		}
	}
}

int main(int argc, char** argv)
{
	if(argc != 2)
    {
        perror("usage:client <IPaddress>");
        exit(1);
	}
	
	sockaddr_in serveraddr;
	int sockfd;
	
	printf("Start client.\n");
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}
	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	
	if(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) <=0)
	{
		perror("inet_pton error");
		close(sockfd);
		exit(1);
	}
	
	if(connect(sockfd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("connect error");
		exit(1);
	}
	
	pthread_t recv_pid;
	
	if(pthread_create(&recv_pid, NULL, recv_message, &sockfd) < 0)
	{
		perror("pthread_t error");
		close(sockfd);
		exit(1);
	}
	
	char buff[MAX_LINE];
	ssize_t bufflen;
	while(fgets(buff, MAX_LINE, stdin) != NULL)
	{
		if(strcmp(buff, "exit\n") == 0)
		{
			printf("Exit Chart room.\n");
			bzero(buff, MAX_LINE);
			sprintf(buff, "%s", "Byebye.");
			send(sockfd, buff, bufflen, 0);
			close(sockfd);
			break;
		}
		if(send(sockfd, buff, bufflen, 0)<0)
		{
			perror("send error");
			close(sockfd);
			break;
		}
		printf("Client: %s", buff);
	}
	close(sockfd);
	return 0;
}