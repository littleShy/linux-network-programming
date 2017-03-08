# linux-network-programming
linux network programming

```C++
#include <sys/socket.h>
int socket(int family, int type, int protocol);
```
>Protocol family constants for socket function

| family  |  Description|
|---------|:---------:|
|AF_INET  |IPv4 protocols|
|AF_INET6 |IPv6 protocols|
|AF_LOCAL |Unix domain protocols|
|AF_ROUTE |Routing protocols|
|AF_KEY   |Key protocols|

>type of socket for socket function

| type         |  Description   |
|---------     |   :---------:  |
|SOCK_STREAM   |stream socket   |
|SOCK_DGRAM    |datagram socket |
|SOCK_SEQPACKET|sequenced packet socket|
|SOCK_RAW      |raw socket      |

>protocol of sockets for AF_INET or AF_INET6

| Protocol    |  Description   |
|---------    |   :---------:  |
|IPPROTO_TCP  |TCP transport protocol|
|IPPROTO_UDP  |UDP transport protocol|
|IPPROTO_SCTP |SCTP transport protocol|

>Combinations of family and type for the socket function

|              |AF_INET|AF_INET6|AF_LOCAL|AF_ROTUE|AF_KEY|
|--------------|:------:|:------:|:------:|:------:|:----:|
|SOCK_STREAM   |TCP/SCTP|TCP/SCTP|YES|
|SOCK_DGRAM    |UDP     |UDP     |YES|
|SOCK_SEQPACKET|SCTP    |SCTP    |YES|
|SOCK_RAW      |IPv4    |IPv6    |        |YES     |YES|

```C++
#include <sys/socket.h>
 
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
//Returns: 0 if OK, -1 on error
```

```C++
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
//Returns: 0 if OK,-1 on error

```

>If we specify a port number of 0, the kernel chooses an ephemeral port when bind is called. But if we specify a wildcard IP address, the kernel does not choose the local IP address until either the socket is connected (TCP) or a datagram is sent on the socket (UDP).

```c++
#include <sys/socket.h>
 
#int listen (int sockfd, int backlog);
//Returns: 0 if OK, -1 on error
```

>To understand the backlog argument, we must realize that for a given listening socket, the kernel maintains two queues:

>- An incomplete connection queue, which contains an entry for each SYN that has arrived from a client for which the server is awaiting completion of the TCP three-way handshake. These sockets are in the SYN_RCVD state.

>- A completed connection queue, which contains an entry for each client with whom the TCP three-way handshake has completed. These sockets are in the ESTABLISHED state.

```c++
#include <sys/socket.h>
 
int accept (int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
//Returns: non-negative descriptor if OK, -1 on error
```
>This function returns up to three values: an integer return code that is either a new socket descriptor or an error indication, the protocol address of the client process (through the cliaddr pointer), and the size of this address (through the addrlen pointer). If we are not interested in having the protocol address of the client returned, we set both cliaddr and addrlen to null pointers.

```c++
#include <unistd.h>
 
int close (int sockfd);
//Returns: 0 if OK, -1 on error
```

>At the end of Section 4.8, we mentioned that when the parent process in our concurrent server closes the connected socket, this just decrements the reference count for the descriptor. Since the reference count was still greater than 0, this call to close did not initiate TCP's four-packet connection termination sequence. This is the behavior we want with our concurrent server with the connected socket that is shared between the parent and child.

>If we really want to send a FIN on a TCP connection, the shutdown function can be used instead of close.

>We must also be aware of what happens in our concurrent server if the parent does not call close for each connected socket returned by accept. First, the parent will eventually run out of descriptors, as there is usually a limit to the number of descriptors that any process can have open at any time. But more importantly, none of the client connections will be terminated. When the child closes the connected socket, its reference count will go from 2 to 1 and it will remain at 1 since the parent never closes the connected socket. This will prevent TCP's connection termination sequence from occurring, and the connection will remain open.

```c++
#include <sys/socket.h>
 
int getsockname(int sockfd, struct sockaddr *localaddr, socklen_t *addrlen);
 
int getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t *addrlen);
//Both return: 0 if OK, -1 on error
```
>These two functions return either the local protocol address associated with a socket (`getsockname`) or the foreign protocol address associated with a socket (`getpeername`).


>
