#include "stream_mqtt.h"
#include "stringbuf.h"

#include <string.h>

int64_t StreamReadMqttString(char **s, size_t *len, Stream *stream)
{
    StringBuf buf;
    int64_t rv;

    if ((rv = StreamReadMqttStringBuf(&buf, stream)) == -1)
        return -1;

    *s = buf.data;
    *len = buf.len;

    return rv;
}

int64_t StreamWriteMqttString(const char *s, int len, Stream *stream)
{
    StringBuf buf;

    if (len < 0)
        len = strlen(s);

    buf.data = (char *) s;
    buf.len = len;

    return StreamWriteMqttStringBuf(&buf, stream);
}

int64_t StreamReadMqttStringBuf(struct StringBuf *buf, Stream *stream)
{
    uint16_t len;

    if (StreamReadUint16Be(&len, stream) == -1)
        return -1;

    if (StringBufInit(buf, len) == -1)
        return -1;

    if (StreamRead(buf->data, len, stream) == -1)
    {
        StringBufDeinit(buf);
        return -1;
    }

    buf->len = len;

    return len+2;
}

int64_t StreamWriteMqttStringBuf(const struct StringBuf *buf, Stream *stream)
{
    if (StreamWriteUint16Be(buf->len, stream) == -1)
        return -1;

    if (StreamWrite(buf->data, buf->len, stream) == -1)
        return -1;

    return 2 + buf->len;
}

int64_t StreamReadRemainingLength(size_t *remainingLength, Stream *stream)
{
    size_t multiplier = 1;
    unsigned char encodedByte;
    *remainingLength = 0;
    do
    {
        if (StreamRead(&encodedByte, 1, stream) != 1)
            return -1;
        *remainingLength += (encodedByte & 127) * multiplier;
        if (multiplier > 128*128*128)
            return -1;
        multiplier *= 128;
    }
    while ((encodedByte & 128) != 0);
    return 0;
}

int64_t StreamWriteRemainingLength(size_t remainingLength, Stream *stream)
{
    size_t nbytes = 0;
    do
    {
        unsigned char encodedByte = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
            encodedByte |= 128;
        if (StreamWrite(&encodedByte, 1, stream) != 1)
            return -1;
        ++nbytes;
    }
    while (remainingLength > 0);
    return nbytes;
}
