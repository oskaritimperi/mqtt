#include "deserialize.h"
#include "packet.h"
#include "stream_mqtt.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>

typedef int (*MqttPacketDeserializeFunc)(MqttPacket **packet, Stream *stream);

static int MqttPacketWithIdDeserialize(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamReadUint16Be(&(*packet)->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketConnAckDeserialize(MqttPacketConnAck **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamRead(&(*packet)->connAckFlags, 1, stream) != 1)
        return -1;

    if (StreamRead(&(*packet)->returnCode, 1, stream) != 1)
        return -1;

    return 0;
}

static int MqttPacketSubAckDeserialize(MqttPacketSubAck **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    /* 2 bytes for packet id and 1 byte for single return code */
    if (remainingLength != 3)
        return -1;

    if (StreamReadUint16Be(&((*packet)->base.id), stream) == -1)
        return -1;

    if (StreamRead(&((*packet)->returnCode), 1, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketTypeUnsubAckDeserialize(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (remainingLength != 2)
        return -1;

    if (StreamReadUint16Be(&(*packet)->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketPublishDeserialize(MqttPacketPublish **packet, Stream *stream)
{
    size_t remainingLength = 0;
    size_t payloadSize = 0;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    if (StreamReadMqttString(&(*packet)->topicName, stream) == -1)
        return -1;

    LOG_DEBUG("remainingLength:%lu", remainingLength);

    payloadSize = remainingLength - blength((*packet)->topicName) - 2;

    LOG_DEBUG("qos:%d payloadSize:%lu", MqttPacketPublishQos(*packet),
        payloadSize);

    if (MqttPacketHasId((const MqttPacket *) *packet))
    {
        LOG_DEBUG("packet has id");
        payloadSize -= 2;
        if (StreamReadUint16Be(&((*packet)->base.id), stream) == -1)
        {
            return -1;
        }
    }

    LOG_DEBUG("reading payload payloadSize:%lu\n", payloadSize);

    /* Allocate extra byte for a NULL terminator. If the user tries to print
       the payload directly. */

    (*packet)->message = bfromcstralloc(payloadSize+1, "");

    if (StreamRead(bdata((*packet)->message), payloadSize, stream) == -1)
        return -1;

    (*packet)->message->slen = payloadSize;
    (*packet)->message->data[payloadSize] = '\0';

    return 0;
}

static int MqttPacketGenericDeserializer(MqttPacket **packet, Stream *stream)
{
    size_t remainingLength = 0;
    char buffer[256];

    (void) packet;

    if (StreamReadRemainingLength(&remainingLength, stream) == -1)
        return -1;

    while (remainingLength > 0)
    {
        size_t l = sizeof(buffer);

        if (remainingLength < l)
            l = remainingLength;

        if (StreamRead(buffer, l, stream) != (int64_t) l)
            return -1;

        remainingLength -= l;
    }

    return 0;
}

static int ValidateFlags(int type, int flags)
{
    int rv = 0;

    switch (type)
    {
        case MqttPacketTypePublish:
        {
            int qos = (flags >> 1) & 2;
            if (qos >= 0 && qos <= 2)
                rv = 1;
            break;
        }

        case MqttPacketTypePubRel:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeUnsubscribe:
            if (flags == 2)
            {
                rv = 1;
            }
            break;

        default:
            if (flags == 0)
            {
                rv = 1;
            }
            break;
    }

    return rv;
}

int MqttPacketDeserialize(MqttPacket **packet, Stream *stream)
{
    MqttPacketDeserializeFunc deserializer = NULL;
    char typeAndFlags;
    int type;
    int flags;
    int rv;

    if (StreamRead(&typeAndFlags, 1, stream) != 1)
        return -1;

    type = (typeAndFlags & 0xF0) >> 4;
    flags = (typeAndFlags & 0x0F);

    if (!ValidateFlags(type, flags))
    {
        return -1;
    }

    switch (type)
    {
        case MqttPacketTypeConnect:
            break;

        case MqttPacketTypeConnAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketConnAckDeserialize;
            break;

        case MqttPacketTypePublish:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketPublishDeserialize;
            break;

        case MqttPacketTypePubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubRec:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubRel:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypePubComp:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketWithIdDeserialize;
            break;

        case MqttPacketTypeSubscribe:
            break;

        case MqttPacketTypeSubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketSubAckDeserialize;
            break;

        case MqttPacketTypeUnsubscribe:
            break;

        case MqttPacketTypeUnsubAck:
            deserializer = (MqttPacketDeserializeFunc) MqttPacketTypeUnsubAckDeserialize;
            break;

        case MqttPacketTypePingReq:
            break;

        case MqttPacketTypePingResp:
            break;

        case MqttPacketTypeDisconnect:
            break;

        default:
            return -1;
    }

    if (!deserializer)
    {
        deserializer = MqttPacketGenericDeserializer;
    }

    *packet = MqttPacketNew(type);

    if (!*packet)
        return -1;

    if (type == MqttPacketTypePublish)
    {
        MqttPacketPublishDup(*packet) = (flags >> 3) & 1;
        MqttPacketPublishQos(*packet) = (flags >> 1) & 3;
        MqttPacketPublishRetain(*packet) = flags & 1;
    }

    rv = deserializer(packet, stream);

    return rv;
}
