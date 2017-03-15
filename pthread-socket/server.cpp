#include "config.h"

void *recv_message(void *pfd)
{
	int connfd = *(int *)pfd;
	char buff[MAX_LINE];
	ssize_t bufflen;
	while(1)
	{
		if((bufflen = recv(connfd, buff, MAX_LINE, 0)) < 0)
		{
			perror("recv error");
			exit(1);
		}
		if(bufflen == 0)
		{
			printf("Client is down.\n");
			close(connfd);
			exit(0);
		}
		//buff[bufflen] = '\0';
		printf("Client:%s", buff);
		if(strcmp(buff, "byebye") == 0)
		{
			printf("Client Closed.\n");
			close(connfd);
			exit(0);
		}
	}
	
}


int main()
{
	int listenfd, connfd;
	socklen_t clilen;
	sockaddr_in serveraddr, clientaddr;
	
	printf("Start Server.\n");
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}
	printf("Create Socket.\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	
	if(bind(listenfd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("bind error");
		close(listenfd);
		exit(1);
	}
	printf("Bind Port:%d\n", PORT);
	
	if(listen(listenfd, LISTENQ) < 0)
	{
		perror("listen error");
		close(listenfd);
		exit(1);
	}
	
	printf("Listen.\n");
	
	clilen = sizeof(clientaddr);
	if((connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen)) < 0)
	{
		perror("accept error");
		close(listenfd);
		exit(1);
	}
	
	printf("New Connection from [%s:%d].\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
	
	pthread_t recv_pid;
	if(pthread_create(&recv_pid, NULL, recv_message, &connfd) < 0)
	{
		perror("pthread error");
		close(connfd);
		close(listenfd);
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
			send(connfd, buff, bufflen, 0);
			close(connfd);
			break;
		}
		if(send(connfd, buff, bufflen, 0)<0)
		{
			perror("send error");
			close(connfd);
			break;
		}
		printf("Server: %s", buff);
	}
	close(listenfd);
	return 0;
}