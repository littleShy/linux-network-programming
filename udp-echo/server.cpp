#include "config.h"

int dg_echo(int sockFd, sockaddr *paddr, socklen_t addrlen)
{
	socklen_t len = addrlen;
	for ( ; ; )
	{
		int n;
		char msg[MAX_LINE];
		if((n = recvfrom(sockFd, msg, MAX_LINE, 0, paddr, &len)) < 0)
		{
			perror("recvfrom error.");
			exit(1);
		}
		if(n == 0)
		{
			printf("end.\n");
			break;
		}
		printf("recv:%s", msg);
		
		if((n = sendto(sockFd, msg, n, 0, paddr, len)) < 0)
		{
			perror("sendto error.");
			exit(1);
		}
		printf("send back.\n");
	}
}

int main(int argc, char **argv)
{
	sockaddr_in serveraddr, clientaddr;
	int sockFd;
	
	printf("Start UDP echo.\n");
	
	if((sockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket error.");
		exit(1);
	}
	
	printf("Create Socket.\n");
	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	
	if(bind(sockFd, (sockaddr*)&serveraddr, sizeof(sockaddr)) < 0)
	{
		perror("bind error.");
		exit(1);
	}
	
	printf("Bind port: %d.\n", PORT);
	dg_echo(sockFd, (sockaddr *)&clientaddr, sizeof(sockaddr));
	close(sockFd);
	return 0;
}
