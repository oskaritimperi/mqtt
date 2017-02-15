#include "stream_mqtt.h"

#include <string.h>

int64_t StreamReadMqttString(bstring *buf, Stream *stream)
{
    uint16_t len;
    bstring result;

    if (StreamReadUint16Be(&len, stream) == -1)
        return -1;

    /* We need 1 extra byte for a NULL terminator. bfromcstralloc doesn't do
       any size snapping. */
    result = bfromcstralloc(len+1, "");

    if (!result)
        return -1;

    if (StreamRead(bdata(result), len, stream) == -1)
    {
        bdestroy(result);
        return -1;
    }

    result->slen = len;
    result->data[len] = '\0';

    *buf = result;

    return len+2;
}

int64_t StreamWriteMqttString(const_bstring buf, Stream *stream)
{
    if (StreamWriteUint16Be(blength(buf), stream) == -1)
        return -1;

    if (StreamWrite(bdata(buf), blength(buf), stream) == -1)
        return -1;

    return 2 + blength(buf);
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
