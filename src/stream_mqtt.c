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

int64_t StreamReadRemainingLength(size_t *remainingLength, size_t *mul,
                                  Stream *stream)
{
    unsigned char encodedByte;
    do
    {
        if (StreamRead(&encodedByte, 1, stream) != 1)
            return -1;
        *remainingLength += (encodedByte & 127) * (*mul);
        if ((*mul) > 128*128*128)
            return -1;
        (*mul) *= 128;
    }
    while ((encodedByte & 128) != 0);
    *mul = 0;
    return 0;
}

int64_t StreamWriteRemainingLength(size_t *remainingLength, Stream *stream)
{
    do
    {
        size_t tmp = *remainingLength;
        unsigned char encodedByte = tmp % 128;
        tmp /= 128;
        if (tmp > 0)
            encodedByte |= 128;
        if (StreamWrite(&encodedByte, 1, stream) != 1)
        {
            return -1;
        }
        *remainingLength = tmp;
    }
    while (*remainingLength > 0);
    return 0;
}
