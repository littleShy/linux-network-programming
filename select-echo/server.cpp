#include "config.h"

int main()
{
	int sockFd,clientFd,client[FD_SETSIZE];
	fd_set allset, rset;
	int nready, maxfd, maxi=-1;
	socklen_t clilen;
	sockaddr_in serveraddr, clientaddr;
	printf("Start Server.\n");
	
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error.");
		exit(1);
	}
	printf("Create socket.\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	
	if(bind(sockFd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("bind error.");
		exit(1);
	}
	printf("Bind Port:%d\n", PORT);
	
	if(listen(sockFd, LISTENQ) < 0)
	{
		perror("listen error.");
		exit(1);
	}
	printf("Listen.\n");
	
	FD_ZERO(&allset);
	FD_SET(sockFd, &allset);
	maxfd = sockFd;
	for(int i=0;i<FD_SETSIZE;i++)
	{
		client[i] = -1;
	}
	
	while(1)
	{
		rset = allset;
		if((nready = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0)
		{
			perror("select error");
			exit(1);
		}
		if(FD_ISSET(sockFd, &rset))
		{
			clilen = sizeof(sockaddr);
			int i;
			if((clientFd = accept(sockFd, (sockaddr *)&clientaddr, &clilen)) < 0)
			{
				perror("accept error.");
				exit(1);
			}
			printf("New client[%s:%d] connected.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
			
			for(i=0;i<FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = clientFd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				perror("Too many connection.");
				exit(1);
			}
			
			FD_SET(clientFd, &allset);
			maxfd = maxfd > clientFd ? maxfd : clientFd;
			maxi = maxi > i ? maxi : i;
			
			if(--nready <= 0)
				continue;
		}
		for(int i=0;i<=maxi;++i)
		{
			if(client[i] < 0)
				continue;
			
			if(FD_ISSET(client[i], &rset))
			{
				if(getpeername(client[i], (sockaddr *)&clientaddr, &clilen) < 0)
				{
					perror("getpeername error.");
					exit(1);
				}
				printf("Receive from[%s:%d]:\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
				
				int buffLen;
				char buff[MAX_LINE];
				if((buffLen = read(client[i], buff, MAX_LINE)) <= 0)
				{
					close(client[i]);
					client[i] = -1;
					FD_CLR(client[i], &allset);
				}
				else
				{
					printf("\t%s", buff);
					printf("Send msg back.\n");
					if(write(client[i], buff, buffLen) < 0)
					{
						perror("write error.");
					}
				}
				if(--nready <= 0)
					continue;
			}
		}
	}
	return 0;
}