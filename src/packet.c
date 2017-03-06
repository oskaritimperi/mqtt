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

MqttPacket *MqttPacketNew(int type)
{
    MqttPacket *packet = NULL;

    packet = (MqttPacket *) calloc(1, sizeof(*packet));
    if (!packet)
        return NULL;

    packet->type = type;

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
    bdestroy(packet->payload);
    free(packet);
}
