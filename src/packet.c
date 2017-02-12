#include "packet.h"
#include "log.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

const char *MqttPacketName(int type)
{
    switch (type)
    {
        case MqttPacketTypeConnect: return "CONNECT";
        case MqttPacketTypeConnAck: return "CONNACK";
        case MqttPacketTypePublish: return "PUBLISH";
        case MqttPacketTypePubAck: return "PUBACK";
        case MqttPacketTypePubRec: return "PUBREC";
        case MqttPacketTypePubRel: return "PUBREL";
        case MqttPacketTypePubComp: return "PUBCOMP";
        case MqttPacketTypeSubscribe: return "SUBSCRIBE";
        case MqttPacketTypeSubAck: return "SUBACK";
        case MqttPacketTypeUnsubscribe: return "UNSUBSCRIBE";
        case MqttPacketTypeUnsubAck: return "UNSUBACK";
        case MqttPacketTypePingReq: return "PINGREQ";
        case MqttPacketTypePingResp: return "PINGRESP";
        case MqttPacketTypeDisconnect: return "DISCONNECT";
        default: return NULL;
    }
}

static inline size_t MqttPacketStructSize(int type)
{
    switch (type)
    {
        case MqttPacketTypeConnect: return sizeof(MqttPacketConnect);
        case MqttPacketTypeConnAck: return sizeof(MqttPacketConnAck);
        case MqttPacketTypePublish: return sizeof(MqttPacketPublish);
        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp: return sizeof(MqttPacket);
        case MqttPacketTypeSubscribe: return sizeof(MqttPacketSubscribe);
        case MqttPacketTypeSubAck: return sizeof(MqttPacketSubAck);
        case MqttPacketTypeUnsubscribe: return sizeof(MqttPacketUnsubscribe);
        case MqttPacketTypeUnsubAck: return sizeof(MqttPacket);
        case MqttPacketTypePingReq: return sizeof(MqttPacket);
        case MqttPacketTypePingResp: return sizeof(MqttPacket);
        case MqttPacketTypeDisconnect: return sizeof(MqttPacket);
        default: return (size_t) -1;
    }
}

MqttPacket *MqttPacketNew(int type)
{
    MqttPacket *packet = NULL;

    packet = (MqttPacket *) calloc(1, MqttPacketStructSize(type));
    if (!packet)
        return NULL;

    packet->type = type;

    /* this will make sure that TAILQ_PREV does not segfault if a message
       has not been added to a list at any point */
    packet->messages.tqe_prev = &packet->messages.tqe_next;

    return packet;
}

MqttPacket *MqttPacketWithIdNew(int type, uint16_t id)
{
    MqttPacket *packet = MqttPacketNew(type);
    if (!packet)
        return NULL;
    packet->id = id;
    return packet;
}

void MqttPacketFree(MqttPacket *packet)
{
    /* TODO: implement MqttPacketFree */
}

int MqttPacketHasId(const MqttPacket *packet)
{
    switch (packet->type)
    {
        case MqttPacketTypePublish:
            return MqttPacketPublishQos(packet) > 0;

        case MqttPacketTypePubAck:
        case MqttPacketTypePubRec:
        case MqttPacketTypePubRel:
        case MqttPacketTypePubComp:
        case MqttPacketTypeSubscribe:
        case MqttPacketTypeSubAck:
        case MqttPacketTypeUnsubscribe:
        case MqttPacketTypeUnsubAck:
            return 1;

        default:
            return 0;
    }
}
