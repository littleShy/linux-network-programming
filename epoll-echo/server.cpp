#include "config.h"

int main()
{
	sockaddr_in serveraddr, clientaddr;
	int sockFd, epfd, nfds;
	epoll_event ev, events[CONNECT_SIZE];
	socklen_t clilen;
	
	printf("Start server.\n");
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}    
	printf("Create Socket.\n");
	
	if((epfd = epoll_create(CONNECT_SIZE)) < 0)
	{
		perror("epoll_create error");
		close(sockFd);
		exit(1);
	}
	
	printf("Create epoll.\n");
	ev.data.fd = sockFd;
	ev.events = EPOLLIN | EPOLLET;
	
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd, &ev) < 0)
	{
		perror("epoll_ctl error");
		exit(1);
	}
	
	bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	
	if(bind(sockFd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
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
	printf("Listen.\n");
	
	while(1)
	{
		if((nfds = epoll_wait(epfd, events, CONNECT_SIZE, -1)) < 0)
		{
			perror("epoll_wait error");
			continue;
		}
		for(int i=0;i<nfds;++i)
		{
			if(events[i].data.fd == sockFd)
			{
				int newClientFd;
				clilen = sizeof(clientaddr);
				if((newClientFd = accept(sockFd, (sockaddr* )&clientaddr, &clilen)) < 0)
				{
					perror("accept error");
					continue;
				}
				printf("New Client [%s:%d] connected.\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
				
				ev.data.fd = newClientFd;
				ev.events = EPOLLIN | EPOLLET;
				if(epoll_ctl(epfd, EPOLL_CTL_ADD, newClientFd, &ev) < 0)
				{
					perror("epoll_ctl error");
					continue;
				}
			}
			else
			{
				char buff[MAX_LINE];
				ssize_t buffLen;
				if(events[i].events & EPOLLIN)
				{
					if((buffLen = read(events[i].data.fd, buff, MAX_LINE)) < 0)
					{
						perror("read error");
						continue;
					}
					else if(buffLen == 0)
					{
						printf("Connect closed from client[%s:%d].\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
						close(events[i].data.fd);
						events[i].data.fd = -1;
						continue;
					}
					clilen = sizeof(clientaddr);
					bzero(&clientaddr, clilen);
					if(getpeername(events[i].data.fd, (sockaddr *)&clientaddr, &clilen) < 0)
					{
						perror("getpeername error");
						printf("Read :%s", buff);
					}
					else
					{
						printf("Read from [%s:%d]:%s", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, buff);
					}
					printf("Send back.\n");
					
					if(write(events[i].data.fd, buff, buffLen) != buffLen)
					{
						perror("Write error");
						close(events[i].data.fd);
						events[i].data.fd = -1;
						break;
					}
				}
			}
		}
	}
	free(events);
	close(epfd);


	exit(0);
}