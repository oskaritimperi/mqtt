#ifndef SOCKET_H
#define SOCKET_H

#include "config.h"

#include <stdlib.h>
#include <stdint.h>

int SocketConnect(const char *host, short port);

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

#endif
