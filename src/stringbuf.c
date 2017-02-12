#include "stringbuf.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

int StringBufInit(StringBuf *buf, size_t size)
{
    assert(buf != NULL);
    memset(buf, 0, sizeof(*buf));
    return StringBufGrow(buf, size);
}

int StringBufInitFromCString(StringBuf *buf, const char *s, int len)
{
    if (len < 0)
        len = strlen(s);
    return StringBufInitFromData(buf, s, len);
}

int StringBufInitFromData(StringBuf *buf, const void *ptr, size_t size)
{
    if (StringBufInit(buf, size) != 0)
        return -1;
    memcpy(buf->data, ptr, size);
    buf->len = size;
    return 0;
}

void StringBufDeinit(StringBuf *buf)
{
    assert(buf != NULL);
    if (buf->size > 0 && buf->data)
        free(buf->data);
    memset(buf, 0, sizeof(*buf));
}

size_t StringBufAvailable(StringBuf *buf)
{
    assert(buf != NULL);
    assert(buf->data != NULL);
    assert(buf->len <= buf->size);
    return buf->size - buf->len;
}

int StringBufGrow(StringBuf *buf, size_t size)
{
    size_t newSize;
    char *ptr;
    assert(buf != NULL);
    newSize = buf->size + size;
    ptr = realloc(buf->data, newSize+1);
    if (!ptr)
        return -1;
    buf->data = ptr;
    buf->size = newSize;
    buf->data[buf->size] = '\0';
    return 0;
}

int StringBufAppendData(StringBuf *buf, const void *ptr, size_t size)
{
    assert(buf != NULL);
    assert(buf->data != NULL);
    assert(ptr != NULL);
    assert(size > 0);
    if (StringBufAvailable(buf) < size)
    {
        if (StringBufGrow(buf, size) == -1)
            return -1;
    }
    memcpy(buf->data + buf->len, ptr, size);
    buf->len += size;
    return 0;
}
