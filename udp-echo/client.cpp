#include "config.h"

int dg_echo(int sockFd, sockaddr *paddr, socklen_t addrlen)
{
	char buff[MAX_LINE];
	while(fgets(buff, MAX_LINE, stdin))
	{
		ssize_t n;
		socklen_t len;
		if((n = sendto(sockFd, buff, strlen(buff), 0, paddr, addrlen)) < 0)
		{
			perror("sendto error.");
			exit(1);
		}
		
		printf("send:%s", buff);
		
		bzero(buff, MAX_LINE);
		if((n = recvfrom(sockFd, buff, MAX_LINE , 0 , NULL , NULL)) < 0)
		{
			perror("recvfrom error.");
			exit(1);
		}
		printf("recv:%s", buff);
		bzero(buff, MAX_LINE);
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		perror("usage: udpcli <IPaddress>");
		exit(1);
	}
	sockaddr_in serveraddr;
	int sockFd;
	
	printf("Start UDP echo Client.\n");
	
	if((sockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket error.");
		exit(1);
	}
	
	printf("Create Socket.\n");
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	
	if(inet_pton(AF_INET , argv[1], &(serveraddr.sin_addr)) <= 0)
	{
		perror("inet_pton error.");
		exit(1);
	}
	
	dg_echo(sockFd, (sockaddr *)&serveraddr, sizeof(sockaddr));
	
	return 0;
}