# linux-network-programming

[TOC]

---

####Function `socket`

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

---

####Function `connect`

```C++
#include <sys/socket.h>
 
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
//Returns: 0 if OK, -1 on error
```
---

####Function `bind`

```C++
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
//Returns: 0 if OK,-1 on error

```

>If we specify a port number of 0, the kernel chooses an ephemeral port when bind is called. But if we specify a wildcard IP address, the kernel does not choose the local IP address until either the socket is connected (TCP) or a datagram is sent on the socket (UDP).

---

####Function `listen`

```c++
#include <sys/socket.h>
 
#int listen (int sockfd, int backlog);
//Returns: 0 if OK, -1 on error
```

>To understand the backlog argument, we must realize that for a given listening socket, the kernel maintains two queues:

>- An incomplete connection queue, which contains an entry for each SYN that has arrived from a client for which the server is awaiting completion of the TCP three-way handshake. These sockets are in the SYN_RCVD state.
>- A completed connection queue, which contains an entry for each client with whom the TCP three-way handshake has completed. These sockets are in the ESTABLISHED state.

---

####Function `accept`

```c++
#include <sys/socket.h>
 
int accept (int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
//Returns: non-negative descriptor if OK, -1 on error
```
>This function returns up to three values: an integer return code that is either a new socket descriptor or an error indication, the protocol address of the client process (through the cliaddr pointer), and the size of this address (through the addrlen pointer). If we are not interested in having the protocol address of the client returned, we set both cliaddr and addrlen to null pointers.

---

####Function `close`

```c++
#include <unistd.h>
 
int close (int sockfd);
//Returns: 0 if OK, -1 on error
```

>At the end of Section 4.8, we mentioned that when the parent process in our concurrent server closes the connected socket, this just decrements the reference count for the descriptor. Since the reference count was still greater than 0, this call to close did not initiate TCP's four-packet connection termination sequence. This is the behavior we want with our concurrent server with the connected socket that is shared between the parent and child.

>If we really want to send a FIN on a TCP connection, the shutdown function can be used instead of close.

>We must also be aware of what happens in our concurrent server if the parent does not call close for each connected socket returned by accept. First, the parent will eventually run out of descriptors, as there is usually a limit to the number of descriptors that any process can have open at any time. But more importantly, none of the client connections will be terminated. When the child closes the connected socket, its reference count will go from 2 to 1 and it will remain at 1 since the parent never closes the connected socket. This will prevent TCP's connection termination sequence from occurring, and the connection will remain open.

---

####Function `getsockname`/`getpeername`
```c++
#include <sys/socket.h>
 
int getsockname(int sockfd, struct sockaddr *localaddr, socklen_t *addrlen);
 
int getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t *addrlen);
//Both return: 0 if OK, -1 on error
```
>These two functions return either the local protocol address associated with a socket (`getsockname`) or the foreign protocol address associated with a socket (`getpeername`).

---

####Function `select`

```c++
#include <sys/select.h>
 
#include <sys/time.h>
 
int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);
//Returns: positive count of ready descriptors, 0 on timeout, –1 on error
```

>We start our description of this function with its final argument, which tells the kernel how long to wait for one of the specified descriptors to become ready. A timeval structure specifies the number of seconds and microseconds.

#####Struct `timeval`
```c++
struct timeval  {
  long   tv_sec;          /* seconds */
  long   tv_usec;         /* microseconds */
};
```

>There are three possibilities:

>- Wait forever— Return only when one of the specified descriptors is ready for I/O. For this, we specify the timeout argument as a null pointer.
>- Wait up to a fixed amount of time— Return when one of the specified descriptors is ready for I/O, but do not wait beyond the number of seconds and microseconds specified in the timeval structure pointed to by the timeout argument.
>- Do not wait at all— Return immediately after checking the descriptors. This is called polling. To specify this, the timeout argument must point to a timeval structure and the timer value (the number of seconds and microseconds specified by the structure) must be 0.

>The wait in the first two scenarios is normally interrupted if the process catches a signal and returns from the signal handler.

>The three middle arguments, readset, writeset, and exceptset, specify the descriptors that we want the kernel to test for reading, writing, and exception conditions. There are only two exception conditions currently supported:

>- The arrival of out-of-band data for a socket. We will describe this in more detail in Chapter 24.
>- The presence of control status information to be read from the master side of a pseudo-terminal that has been put into packet mode.

#####`fd_set` macros
```c++
void FD_ZERO(fd_set *fdset);
 /* clear all bits in fdset */
 
void FD_SET(int fd, fd_set *fdset);
 /* turn on the bit for fd in fdset */
 
void FD_CLR(int fd, fd_set *fdset);
 /* turn off the bit for fd in fdset */
 
int FD_ISSET(int fd, fd_set *fdset);
 /* is the bit for fd on in fdset ? */
```

>We allocate a descriptor set of the fd_set datatype, we set and test the bits in the set using these macros, and we can also assign it to another descriptor set across an equals sign (=) in C.
>For example, to define a variable of type fd_set and then turn on the bits for descriptors 1, 4, and 5, we write

```c++
fd_set rset;

FD_ZERO(&rset);          /* initialize the set: all bits off */
FD_SET(1, &rset);        /* turn on bit for fd 1 */
FD_SET(4, &rset);        /* turn on bit for fd 4 */
FD_SET(5, &rset);        /* turn on bit for fd 5 */
```

>It is important to initialize the set, since unpredictable results can occur if the set is allocated as an automatic variable and not initialized.

>Any of the middle three arguments to select, readset, writeset, or exceptset, can be specified as a null pointer if we are not interested in that condition. Indeed, if all three pointers are null, then we have a higher precision timer than the normal Unix sleep function

> The maxfdp1 argument specifies the number of descriptors to be tested. Its value is the maximum descriptor to be tested plus one (hence our name of maxfdp1). The descriptors 0, 1, 2, up through and including maxfdp1–1 are tested. The reason it is maximum descriptor plus one and not maximum descriptor is that we are specifying the number of descriptors, not the largest value, and descriptors start at 0.

>select modifies the descriptor sets pointed to by the readset, writeset, and exceptset pointers. These three arguments are value-result arguments. When we call the function, we specify the values of the descriptors that we are interested in, and on return, the result indicates which descriptors are ready. We use the FD_ISSET macro on return to test a specific descriptor in an fd_set structure. Any descriptor that is not ready on return will have its corresponding bit cleared in the descriptor set. To handle this, we turn on all the bits in which we are interested in all the descriptor sets each time we call select.

#####Under What Conditions Is a Descriptor Ready?

>* A socket is ready for reading if any of the following four conditions is true:
 >- The number of bytes of data in the socket receive buffer is greater than or equal to the current size of the low-water mark for the socket receive buffer. A read operation on the socket will not block and will return a value greater than 0 (i.e., the data that is ready to be read). We can set this low-water mark using the SO_RCVLOWAT socket option. It defaults to 1 for TCP and UDP sockets.
 >- The read half of the connection is closed (i.e., a TCP connection that has received a FIN). A read operation on the socket will not block and will return 0 (i.e., EOF).
 >- The socket is a listening socket and the number of completed connections is nonzero. An accept on the listening socket will normally not block, although we will describe a timing condition in Section 16.6 under which the accept can block.
 >- A socket error is pending. A read operation on the socket will not block and will return an error (–1) with errno set to the specific error condition. These pending errors can also be fetched and cleared by calling getsockopt and specifying the SO_ERROR socket option.
>* A socket is ready for writing if any of the following four conditions is true:

 >- The number of bytes of available space in the socket send buffer is greater than or equal to the current size of the low-water mark for the socket send buffer and either: (i) the socket is connected, or (ii) the socket does not require a connection (e.g., UDP). This means that if we set the socket to nonblocking (Chapter 16), a write operation will not block and will return a positive value (e.g., the number of bytes accepted by the transport layer). We can set this low-water mark using the SO_SNDLOWAT socket option. This low-water mark normally defaults to 2048 for TCP and UDP sockets.

 >- The write half of the connection is closed. A write operation on the socket will generate SIGPIPE (Section 5.12).

 >- A socket using a non-blocking connect has completed the connection, or the connect has failed.

 >- A socket error is pending. A write operation on the socket will not block and will return an error (–1) with errno set to the specific error condition. These pending errors can also be fetched and cleared by calling getsockopt with the SO_ERROR socket option.

>* A socket has an exception condition pending if there is out-of-band data for the socket or the socket is still at the out-of-band mark. (We will describe out-of-band data in Chapter 24.)

 >- Our definitions of "readable" and "writable" are taken directly from the kernel's soreadable and sowriteable macros on pp. 530–531 of TCPv2. Similarly, our definition of the "exception condition" for a socket is from the soo_select function on these same pages.

---

####Function `poll`
```c++
#include <poll.h>
 
int poll (struct pollfd *fdarray, unsigned long nfds, int timeout);
//Returns: count of ready descriptors, 0 on timeout, –1 on error
```
#####Struct `pollfd`
>The first argument is a pointer to the first element of an array of structures. Each element of the array is a pollfd structure that specifies the conditions to be tested for a given descriptor, fd.

```c++
struct pollfd {
  int     fd;       /* descriptor to check */
  short   events;   /* events of interest on fd */
  short   revents;  /* events that occurred on fd */
};
```

>The conditions to be tested are specified by the events member, and the function returns the status for that descriptor in the corresponding revents member. (Having two variables per descriptor, one a value and one a result, avoids value-result arguments. Recall that the middle three arguments for select are value-result.) Each of these two members is composed of one or more bits that specify a certain condition.

>If we are no longer interested in a particular descriptor, we just set the fd member of the pollfd structure to a negative value. Then the events member is ignored and the revents member is set to 0 on return.

######Input `events` and returned `revents` for `poll`.
|Constant|Input to evnets?|Result from revents?|Description|
|--------|:--------------:|:------------------:|:---------:|
|POLLIN<br>POLLRDNORM<br>POLLRDBAND<br>POLLPRI|\*<br>\*<br>\*<br>\*|\*<br>\*<br>\*<br>\*|Nromal or priority band data can be read<br>Normal data can be read<br>Priority data can be read<br>High-priority data can be read|
|POLLOUT<br>POLLWRNORM<br>POLLWRBAND|\*<br>\*<br>\*|\*<br>\*<br>\*|Normal data can be writen<br>Nromal data can be writen<br>Priority band data can be writen|
|POLLERR<br>POLLHUP<br>POLLNVAL| |\*<br>\*<br>\*|Error has occurred<br>Hungup has occurred<br>Discriptor is not an open file|

>We have divided this figure into three sections: The first four constants deal with input, the next three deal with output, and the final three deal with errors. Notice that the final three cannot be set in events, but are always returned in revents when the corresponding condition exists.

>With regard to TCP and UDP sockets, the following conditions cause poll to return the specified `revent`. Unfortunately, POSIX leaves many holes (i.e., optional ways to return the same condition) in its definition of poll.

>* All regular TCP data and all UDP data is considered normal.
>* TCP's out-of-band data (Chapter 24) is considered priority band.
>* When the read half of a TCP connection is closed (e.g., a FIN is received), this is also considered normal data and a subsequent read operation will return 0.
>* The presence of an error for a TCP connection can be considered either normal data or an error (POLLERR). In either case, a subsequent read will return –1 with errno set to the appropriate value. This handles conditions such as the receipt of an RST or a timeout.
>* The availability of a new connection on a listening socket can be considered either normal data or priority data. Most implementations consider this normal data.
>* The completion of a nonblocking connect is considered to make a socket writable.

>The number of elements in the array of structures is specified by the `nfds` argument.

#####Timeout values for `poll`

|Timeout value|Description|
|-------------|:---------:|
|INFTIM<br>0<br>\>0|Wait forever<br>Return immediately, do not block<br>Wait specified number of milliseconds|

>The constant `INFTIM` is defined to be a negative value. If the system does not provide a timer with millisecond accuracy, the value is rounded up to the nearest supported value.
>* As with select, any timeout set for poll is limited by the implementation's clock resolution (often 10 ms).

>The return value from poll is –1 if an error occurred, 0 if no descriptors are ready before the timer expires, otherwise it is the number of descriptors that have a nonzero revents member.

>FD_SETSIZE and the maximum number of descriptors per descriptor set versus the maximum number of descriptors per process. We do not have that problem with poll since it is the caller's responsibility to allocate an array of pollfd structures and then tell the kernel the number of elements in the array. There is no fixed-size datatype similar to fd_set that the kernel knows about.

>* The POSIX specification requires both select and poll. But, from a portability perspective today, more systems support select than poll. Also, POSIX defines pselect, an enhanced version of select that handles signal blocking and provides increased time resolution. Nothing similar is defined for poll.











