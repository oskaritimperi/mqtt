#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include "stream.h"

typedef struct SocketStream SocketStream;

struct SocketStream
{
    Stream base;
    int sock;
};

int SocketStreamOpen(SocketStream *stream, int sock);

#endif
