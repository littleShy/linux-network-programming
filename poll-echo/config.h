#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <poll.h>
#include <limits.h>		/*for OPEN_MAX*/
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif

#ifndef INFTIM
#define INFTIM -1
#endif

#define PORT 4321
#define MAX_LINE 2048
#define LISTENQ 20
