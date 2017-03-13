#include "config.h"


int readline(int fd, char* buff, ssize_t buffLen)
{
	char c,*pBuff = buff;
	int n = 0;
	for(n=1;n<buffLen;++n)
	{
		int cnt;
		if((cnt = read(fd, &c, 1)) == 0)
		{
			*pBuff = 0;
			return n-1;
		}
		else if(cnt < 0)
		{
			return -1;
		}
		else
		{
			*pBuff++ = c;
			
			if(c == '\n')
			{
				break;
			}
		}
	}
	*pBuff = 0;
	return n;
}

int str_cli(int sockFd)
{
	char buff[MAX_LINE];
	bzero(buff, MAX_LINE);
	while(fgets(buff, MAX_LINE, stdin) != NULL)
	{
		if(write(sockFd, buff, strlen(buff)) != strlen(buff))
		{
			perror("write error");
			close(sockFd);
			exit(1);
		}
		printf("Write to server.\n");
		
		bzero(buff, MAX_LINE);
		if(readline(sockFd, buff, MAX_LINE) < 0)
		{
			perror("readline error");
			close(sockFd);
			exit(1);
		}
		
		printf("Receive from server:%s", buff);
		bzero(buff, MAX_LINE);
	}
	return 0;
}

int str_cli2(int sockFd)
{
	fd_set rset;
	FD_ZERO(&rset);
	while(1)
	{
		FD_SET(sockFd, &rset);
		FD_SET(fileno(stdin), &rset);
		int maxfd = sockFd > fileno(stdin) ? sockFd : fileno(stdin);
		if(select(maxfd+1, &rset, NULL, NULL, NULL) < 0)
		{
			perror("select error");
			continue;
		}
		
		char buff[MAX_LINE];
		bzero(buff, MAX_LINE);
		if(FD_ISSET(fileno(stdin), &rset))
		{
			if(fgets(buff, MAX_LINE, stdin) == NULL)
			{
				perror("fgets error");
				close(sockFd);
				exit(1);
			}
			
			if(write(sockFd, buff, strlen(buff)) != strlen(buff))
			{
				perror("write error");
				close(sockFd);
				exit(1);
			}
		}
		
		bzero(buff, MAX_LINE);
		
		if(FD_ISSET(sockFd, &rset))
		{
			ssize_t readlen;
			if((readlen = read(sockFd, buff, MAX_LINE)) == 0)
			{
				close(sockFd);
				printf("Server is down.\n");
				exit(0);
			}
			else if(readlen < 0)
			{
				perror("read error");
				close(sockFd);
				exit(1);
			}
			else
			{
				printf("Receive from server:%s", buff);
			}
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		perror("usage:client <IPaddress>");
		exit(1);
	}
	
	sockaddr_in serveraddr;
	int sockFd;
	
	printf("Start Client.\n");
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		exit(1);
	}
	
	printf("Create Socket.\n");
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	
	if(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) <= 0)
	{
		perror("inet_pton error");
		exit(1);
	}
	
	if(connect(sockFd, (sockaddr *)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("connect error");
		exit(1);
	}
	printf("Connect Server.\n");
	
	//str_cli(sockFd);
	str_cli2(sockFd);
	return 0;
}