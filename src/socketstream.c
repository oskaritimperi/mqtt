#include "socketstream.h"

#include <assert.h>
#include <string.h>

#if !defined(_WIN32)
#include <arpa/inet.h>
#else
#include "win32.h"
#endif

/* close */
#include <unistd.h>

static int SocketStreamClose(Stream *base)
{
    int rv;
    SocketStream *stream = (SocketStream *) base;
    rv = close(stream->sock);
    stream->sock = -1;
    return rv;
}

static int64_t SocketStreamRead(void *ptr, size_t size, Stream *stream)
{
    SocketStream *ss = (SocketStream *) stream;
    size_t received = 0;
    if (ss->sock == -1)
        return -1;
    while (received < size)
    {
        char *p = ((char *) ptr) + received;
        ssize_t rv = recv(ss->sock, p, size - received, 0);
        /* Error */
        if (rv == -1)
            return -1;
        /* TODO: Closed? */
        if (rv == 0)
            break;
        received += (size_t) rv;
    }
    return received;
}

static int64_t SocketStreamWrite(const void *ptr, size_t size, Stream *stream)
{
    SocketStream *ss = (SocketStream *) stream;
    size_t written = 0;
    if (ss->sock == -1)
        return -1;
    while (written < size)
    {
        const char *p = ((char *) ptr) + written;
        ssize_t rv = send(ss->sock, p, size - written, 0);
        if (rv == -1)
            return -1;
        written += (size_t) rv;
    }
    return written;
}

static const StreamOps SocketStreamOps =
{
    SocketStreamRead,
    SocketStreamWrite,
    SocketStreamClose,
    NULL,
    NULL
};

int SocketStreamOpen(SocketStream *stream, int sock)
{
    assert(stream != NULL);
    assert(sock != -1);
    memset(stream, 0, sizeof(*stream));
    stream->sock = sock;
    stream->base.ops = &SocketStreamOps;
    return 0;
}
