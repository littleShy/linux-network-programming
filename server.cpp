#include "config.h"

int main()
{
	sockaddr_in serveraddr, clientaddr;
	int listenFd, clientFd;
	socklen_t clientLen;
	pid_t childPid;
	printf("Start Server.\n");
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenFd < 0)
	{
		perror("listen error!");
		exit(1);
	}
	printf("Create Socket.\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	bzero(&clientaddr, sizeof(clientaddr));
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);	//表明可以接受任意IP地址
	serveraddr.sin_port = htons(PORT);
	if(bind(listenFd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("bind error!");
		exit(1);
	}
	
	printf("Bind Port %d.\n", PORT);
	
	if(listen(listenFd, LISTENQ) < 0)
	{
		perror("listen error!");
		exit(1);
	}
	
	printf("Listen.\n");
	
	for ( ; ; )
	{
		clientLen = sizeof(clientaddr);
		clientFd = accept(listenFd, (sockaddr*)&clientaddr, &clientLen);
		if(clientFd < 0)
		{
			perror("accept error!");
			exit(1);
		}
		childPid = fork();
		if(childPid == 0)
		{
			close(listenFd);
			ssize_t readLen;
			
			char readBuf[MAX_LINE] = {'\0'};
			while((readLen = read(clientFd, readBuf, MAX_LINE)) > 0)
			{
				printf("read: %s\n", readBuf);
				bzero(readBuf, MAX_LINE);
				write(clientFd, readBuf, readLen);
			}
			close(clientFd);
			exit(0);
		}
		else if(childPid > 0)
		{
			close(clientFd);
		}
		close(clientFd);
	}
	close(listenFd);

	return 0;
}