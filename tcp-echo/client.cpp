#include "config.h"

int readline(int fd, char *buff, const int maxLen)
{
	ssize_t n, recvCnt;
	char *ptr = buff;
	for(n=1;n<maxLen;n++)
	{
		char c;
		recvCnt = read(fd, &c, 1);
		// printf("get c:%c, recv count:%d \n", c, recvCnt);
		if(recvCnt = 1)
		{
			*ptr = c;
			ptr++;
			if(c == '\n')
				break;
		}
		else if(recvCnt == 0)
		{
			*ptr = 0;
			return (n-1);
		}
		else
		{
			perror("read error.");
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		perror("usage:tcpcli <IPaddress>");
		exit(1);
	}
	
	int sockFd;
	sockaddr_in serveraddr;
	printf("Start Client.\n");
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error.");
		exit(1);
	}
	
	printf("Create Socket.\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	if(inet_pton(AF_INET, argv[1], &(serveraddr.sin_addr)) != 1)
	{
		printf("Input Ip address [%s] format is'n valid.\n", argv[1]);
		exit(1);
	}
	
	if(connect(sockFd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("Connect error.");
		exit(1);
	}
	printf("Connect.\n");
	
	char sendBuff[MAX_LINE] = {'\0'}, recvBuff[MAX_LINE] = {'\0'};
	
	while(fgets(sendBuff, MAX_LINE, stdin) != NULL)
	{
		write(sockFd, sendBuff, MAX_LINE);
		readline(sockFd, recvBuff, MAX_LINE);
		printf("recv:%s\n", recvBuff);
	}
	close(sockFd);
	return 0;
}