#include "config.h"

int main()
{
	int sockFd, maxi;
	sockaddr_in serveraddr, clientaddr;
	pollfd client[OPEN_MAX];
	socklen_t clilen;
	
	printf("Start server.\n");
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}
	printf("Create Socket.\n");
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	if(bind(sockFd, (sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	printf("Bind Port:%d\n", PORT);
	
	if(listen(sockFd, LISTENQ) < 0)
	{
		perror("listen error");
		exit(1);
	}
	
	maxi = 0;
	client[0].fd = sockFd;
	client[0].events = POLLRDNORM;
	for(int i=1;i<OPEN_MAX;++i)
	{
		client[i].fd = -1;
	}
	
	while(1)
	{
		int nready;
		if((nready = poll(client, maxi + 1, INFTIM)) < 0)
		{
			perror("poll error");
			close(sockFd);
			exit(1);
		}
		
		if(client[0].revents & POLLRDNORM)
		{
			int cliFd;
			clilen = sizeof(sockaddr);
			if((cliFd = accept(sockFd, (sockaddr *)&clientaddr, &clilen)) < 0)
			{
				perror("accept error");
				exit(1);
			}
			if(getpeername(cliFd, (sockaddr *)&clientaddr, &clilen) < 0)
			{
				perror("getpeername error");
			}
			else
			{
				printf("New client connnected [%s:%d]\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
			}
			int i;
			for(i = 1;i<OPEN_MAX;++i)
			{
				if(client[i].fd < 0)
				{
					client[i].fd = cliFd;
					client[i].events = POLLRDNORM;
					break;
				}
			}
			if(i == OPEN_MAX)
			{
				perror("Too many connection");
				continue;
			}
			maxi = maxi > i ? maxi : i;
			
			if(--nready <= 0)
				continue;
		}
		
		for(int i=1;i<=maxi;++i)
		{
			if(client[i].fd < 0)
				continue;
			if(client[i].revents & (POLLRDNORM | POLLERR))
			{
				char buff[MAX_LINE];
				int n;
				if((n = read(client[i].fd, buff, MAX_LINE)) < 0)
				{
					if(errno == ECONNRESET)
					{
						close(client[i].fd);
						client[i].fd = -1;
					}
					else
					{
						perror("read error");
					}
				}
				else if(n == 0)
				{
					printf("Client has closed connection.\n");
					close(client[i].fd);
					client[i].fd = -1;
				}
				else
				{
					clilen = sizeof(clientaddr);
					if(getpeername(client[i].fd, (sockaddr *)&clientaddr, &clilen) < 0)
					{
						perror("getpeername error");
					}
					else
					{
						printf("Receive msg from [%s:%d]: %s", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, buff);
					}
					if(write(client[i].fd, buff, strlen(buff)) != strlen(buff))
					{
						perror("write error");
					}
					else
					{
						printf("Send msg back.\n");
					}
				}
			}
		}
	}
	return 0;
}