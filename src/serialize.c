#include "serialize.h"
#include "stringbuf.h"
#include "packet.h"
#include "stream_mqtt.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>

typedef int (*MqttPacketSerializeFunc)(const MqttPacket *packet,
                                       Stream *stream);

static const StringBuf MqttProtocolId = StaticStringBuf("MQTT");
static const char MqttProtocolLevel  = 0x04;

static inline size_t MqttStringLengthSerialized(const StringBuf *s)
{
    return 2 + s->len;
}

static size_t MqttPacketConnectGetRemainingLength(const MqttPacketConnect *packet)
{
    size_t remainingLength = 0;

    remainingLength += MqttStringLengthSerialized(&MqttProtocolId) + 1 + 1 + 2;

    remainingLength += MqttStringLengthSerialized(&packet->clientId);

    if (packet->connectFlags & 0x80)
        remainingLength += MqttStringLengthSerialized(&packet->userName);

    if (packet->connectFlags & 0x40)
        remainingLength += MqttStringLengthSerialized(&packet->password);

    if (packet->connectFlags & 0x04)
        remainingLength += MqttStringLengthSerialized(&packet->willTopic) +
                           MqttStringLengthSerialized(&packet->willMessage);

    return remainingLength;
}

static size_t MqttPacketSubscribeGetRemainingLength(const MqttPacketSubscribe *packet)
{
    return 2 + MqttStringLengthSerialized(&packet->topicFilter) + 1;
}

static size_t MqttPacketUnsubscribeGetRemainingLength(const MqttPacketUnsubscribe *packet)
{
    return 2 + MqttStringLengthSerialized(&packet->topicFilter);
}

static size_t MqttPacketPublishGetRemainingLength(const MqttPacketPublish *packet)
{
    size_t remainingLength = 0;

    remainingLength += MqttStringLengthSerialized(&packet->topicName);

    // Packet id
    if (MqttPacketPublishQos(packet) == 1 || MqttPacketPublishQos(packet) == 2)
    {
        remainingLength += 2;
    }

    remainingLength += packet->message.len;

    return remainingLength;
}

static size_t MqttPacketGetRemainingLength(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypeConnect:
            return MqttPacketConnectGetRemainingLength(
                (MqttPacketConnect *) packet);

        case MqttPacketTypeSubscribe:
            return MqttPacketSubscribeGetRemainingLength(
                (MqttPacketSubscribe *) packet);

        case MqttPacketTypePublish:
            return MqttPacketPublishGetRemainingLength(
                (MqttPacketPublish *) packet);

        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp:
            return 2;

        case MqttPacketTypeUnsubscribe:
            return MqttPacketUnsubscribeGetRemainingLength(
                (MqttPacketUnsubscribe *) packet);

        default:
            return 0;
    }
}

static int MqttPacketFlags(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypePublish:
            return ((MqttPacketPublishDup(packet) & 1) << 3) |
                   ((MqttPacketPublishQos(packet) & 3) << 1) |
                   (MqttPacketPublishRetain(packet) & 1);

        case MqttPacketTypePubRel:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeUnsubscribe:
            return 0x2;

        default:
            return 0;
    }
}

static int MqttPacketBaseSerialize(const MqttPacket *packet, Stream *stream)
{
    unsigned char typeAndFlags;
    size_t remainingLength;

    typeAndFlags = ((packet->type & 0x0F) << 4) |
                   (MqttPacketFlags(packet) & 0x0F);
    remainingLength = MqttPacketGetRemainingLength(packet);

    LOG_DEBUG("type:%02X (%s) flags:%02X", packet->type,
        MqttPacketName(packet->type), MqttPacketFlags(packet));

    if (StreamWrite(&typeAndFlags, 1, stream) != 1)
        return -1;

    if (StreamWriteRemainingLength(remainingLength, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketWithIdSerialize(const MqttPacket *packet, Stream *stream)
{
    assert(MqttPacketHasId((const MqttPacket *) packet));

    if (MqttPacketBaseSerialize(packet, stream) == -1)
        return -1;

    if (StreamWriteUint16Be(packet->id, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketConnectSerialize(const MqttPacketConnect *packet, Stream *stream)
{
    if (MqttPacketBaseSerialize(&packet->base, stream) == -1)
        return -1;

    if (StreamWriteMqttStringBuf(&MqttProtocolId, stream) == -1)
        return -1;

    if (StreamWrite(&MqttProtocolLevel, 1, stream) != 1)
        return -1;

    if (StreamWrite(&packet->connectFlags, 1, stream) != 1)
        return -1;

    if (StreamWriteUint16Be(packet->keepAlive, stream) == -1)
        return -1;

    if (StreamWriteMqttStringBuf(&packet->clientId, stream) == -1)
        return -1;

    if (packet->connectFlags & 0x04)
    {
        if (StreamWriteMqttStringBuf(&packet->willTopic, stream) == -1)
            return -1;

        if (StreamWriteMqttStringBuf(&packet->willMessage, stream) == -1)
            return -1;
    }

    if (packet->connectFlags & 0x80)
    {
        if (StreamWriteMqttStringBuf(&packet->userName, stream) == -1)
            return -1;
    }

    if (packet->connectFlags & 0x40)
    {
        if (StreamWriteMqttStringBuf(&packet->password, stream) == -1)
            return -1;
    }

    return 0;
}

static int MqttPacketSubscribeSerialize(const MqttPacketSubscribe *packet, Stream *stream)
{
    if (MqttPacketWithIdSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttStringBuf(&packet->topicFilter, stream) == -1)
        return -1;

    if (StreamWrite(&packet->qos, 1, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketUnsubscribeSerialize(const MqttPacketUnsubscribe *packet, Stream *stream)
{
    if (MqttPacketWithIdSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttStringBuf(&packet->topicFilter, stream) == -1)
        return -1;

    return 0;
}

static int MqttPacketPublishSerialize(const MqttPacketPublish *packet, Stream *stream)
{
    if (MqttPacketBaseSerialize((const MqttPacket *) packet, stream) == -1)
        return -1;

    if (StreamWriteMqttStringBuf(&packet->topicName, stream) == -1)
        return -1;

    LOG_DEBUG("qos:%d", MqttPacketPublishQos(packet));

    if (MqttPacketPublishQos(packet) > 0)
    {
        if (StreamWriteUint16Be(packet->base.id, stream) == -1)
            return -1;
    }

    if (StreamWrite(packet->message.data, packet->message.len, stream) == -1)
        return -1;

    return 0;
}

int MqttPacketSerialize(const MqttPacket *packet, Stream *stream)
{
    MqttPacketSerializeFunc f = NULL;

    switch (packet->type)
    {
        case MqttPacketTypeConnect:
            f = (MqttPacketSerializeFunc) MqttPacketConnectSerialize;
            break;

        case MqttPacketTypeConnAck:
            break;

        case MqttPacketTypePublish:
            f = (MqttPacketSerializeFunc) MqttPacketPublishSerialize;
            break;

        case MqttPacketTypePubAck:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubRec:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubRel:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypePubComp:
            f = (MqttPacketSerializeFunc) MqttPacketWithIdSerialize;
            break;

        case MqttPacketTypeSubscribe:
            f = (MqttPacketSerializeFunc) MqttPacketSubscribeSerialize;
            break;

        case MqttPacketTypeSubAck:
            break;

        case MqttPacketTypeUnsubscribe:
            f = (MqttPacketSerializeFunc) MqttPacketUnsubscribeSerialize;
            break;

        case MqttPacketTypeUnsubAck:
            break;

        case MqttPacketTypePingReq:
            f = (MqttPacketSerializeFunc) MqttPacketBaseSerialize;
            break;

        case MqttPacketTypePingResp:
            break;

        case MqttPacketTypeDisconnect:
            f = (MqttPacketSerializeFunc) MqttPacketBaseSerialize;
            break;

        default:
            return -1;
    }

    assert(f != NULL && "no serializer");

    return f(packet, stream);
}