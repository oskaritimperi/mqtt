#ifndef SOCKET_H
#define SOCKET_H

#include "config.h"

#include <stdlib.h>
#include <stdint.h>

#if defined(_WIN32)
#include "win32.h"
#define SocketErrno (WSAGetLastError())
#define SOCKET_EINPROGRESS (WSAEWOULDBLOCK)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#define SocketErrno (errno)
#define SOCKET_EINPROGRESS (EINPROGRESS)
#endif

int SocketConnect(const char *host, short port, int nonblocking);

int SocketDisconnect(int sock);

int SocketSendAll(int sock, const char *buf, size_t *len);

enum
{
    EV_READ = 1,
    EV_WRITE = 2
};

int SocketSelect(int sock, int *events, int timeout);

int64_t SocketRecv(int sock, void *buf, size_t len, int flags);

int64_t SocketSend(int sock, const void *buf, size_t len, int flags);

void SocketSetNonblocking(int sock, int nb);

int SocketGetError(int sock, int *error);

int SocketWouldBlock(int error);

#endif
