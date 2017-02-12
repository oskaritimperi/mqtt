#include "stream.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// htons, ntohs
#include <arpa/inet.h>

#define STREAM_CHECK_OP(stream, op) \
    do { if ((stream->ops->op) == NULL) \
    { \
        errno = ENOTSUP; \
        return -1; \
    } } while (0)

int StreamClose(Stream *stream)
{
    if (stream->ops->close)
    {
        return stream->ops->close(stream);
    }
    return 0;
}

int64_t StreamRead(void *ptr, size_t size, Stream *stream)
{
    STREAM_CHECK_OP(stream, read);
    int64_t rv = stream->ops->read(ptr, size, stream);
#if defined(STREAM_HEXDUMP_READ)
    if (rv >= 0)
    {
        printf("READ %lu bytes:\n", size);
        DumpHex(ptr, size);
    }
#endif
    return rv;
}

int64_t StreamReadUint16Be(uint16_t *v, Stream *stream)
{
    STREAM_CHECK_OP(stream, read);
    if (StreamRead(v, 2, stream) != 2)
        return -1;
    *v = ntohs(*v);
    return 2;
}

int64_t StreamWrite(const void *ptr, size_t size, Stream *stream)
{
    STREAM_CHECK_OP(stream, write);
#if defined(STREAM_HEXDUMP_WRITE)
    printf("WRITE %lu bytes:\n", size);
    DumpHex(ptr, size);
#endif
    return stream->ops->write(ptr, size, stream);
}

int64_t StreamWriteUint16Be(uint16_t v, Stream *stream)
{
    v = htons(v);
    return StreamWrite(&v, sizeof(v), stream);
}

int StreamSeek(Stream *stream, int64_t offset, int whence)
{
    STREAM_CHECK_OP(stream, seek);
    return stream->ops->seek(stream, offset, whence);
}

int64_t StreamTell(Stream *stream)
{
    STREAM_CHECK_OP(stream, tell);
    return stream->ops->tell(stream);
}
