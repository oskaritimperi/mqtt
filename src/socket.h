#ifndef SOCKET_H
#define SOCKET_H

#include "config.h"

#include <stdlib.h>

int SocketConnect(const char *host, short port);

int SocketDisconnect(int sock);

int SocketSendAll(int sock, const char *buf, size_t *len);

#endif
