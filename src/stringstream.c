#include "stringstream.h"

#include <assert.h>

static int StringStreamClose(Stream *base)
{
    StringStream *ss = (StringStream *) base;
    bdestroy(ss->buffer);
    ss->buffer = NULL;
    return 0;
}

static int64_t StringStreamRead(void *ptr, size_t size, Stream *stream)
{
    StringStream *ss = (StringStream *) stream;
    int64_t available = blength(ss->buffer) - ss->pos;
    void *bufptr;

    if (available <= 0)
    {
        return -1;
    }

    if (size > (size_t) available)
        size = available;

    /* Use a temp buffer pointer to make some warnings disappear when using
       GCC */
    bufptr = bdataofs(ss->buffer, ss->pos);
    memcpy(ptr, bufptr, size);

    ss->pos += size;

    return size;
}

static int64_t StringStreamWrite(const void *ptr, size_t size, Stream *stream)
{
    StringStream *ss = (StringStream *) stream;
    struct tagbstring buf;
    if (ss->buffer->mlen <= 0)
        return -1;
    btfromblk(buf, ptr, size);
    bsetstr(ss->buffer, ss->pos, &buf, '\0');
    ss->pos += size;
    return size;
}

int StringStreamSeek(Stream *base, int64_t offset, int whence)
{
    StringStream *ss = (StringStream *) base;
    int64_t newpos = 0;

    if (whence == SEEK_SET)
    {
        newpos = offset;
    }
    else if (whence == SEEK_CUR)
    {
        newpos = ss->pos + offset;
    }
    else if (whence == SEEK_END)
    {
        newpos = blength(ss->buffer) - offset;
    }
    else
    {
        return -1;
    }

    if (newpos > blength(ss->buffer))
        return -1;

    if (newpos < 0)
        return -1;

    ss->pos = newpos;

    return 0;
}

int64_t StringStreamTell(Stream *base)
{
    StringStream *ss = (StringStream *) base;
    return ss->pos;
}

static const StreamOps StringStreamOps =
{
    StringStreamRead,
    StringStreamWrite,
    StringStreamClose,
    StringStreamSeek,
    StringStreamTell
};

int StringStreamInit(StringStream *stream)
{
    assert(stream != NULL);
    memset(stream, 0, sizeof(*stream));
    stream->pos = 0;
    stream->buffer = bfromcstr("");
    stream->base.ops = &StringStreamOps;
    return 0;
}

int StringStreamInitFromBstring(StringStream *stream, bstring buffer)
{
    assert(stream != NULL);
    memset(stream, 0, sizeof(*stream));
    stream->pos = 0;
    stream->buffer = buffer;
    stream->base.ops = &StringStreamOps;
    return 0;
}
