#ifndef PACKET_H
#define PACKET_H

#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <bstrlib/bstrlib.h>

#include "queue.h"

enum
{
    MqttPacketTypeConnect = 0x1,
    MqttPacketTypeConnAck = 0x2,
    MqttPacketTypePublish = 0x3,
    MqttPacketTypePubAck = 0x4,
    MqttPacketTypePubRec = 0x5,
    MqttPacketTypePubRel = 0x6,
    MqttPacketTypePubComp = 0x7,
    MqttPacketTypeSubscribe = 0x8,
    MqttPacketTypeSubAck = 0x9,
    MqttPacketTypeUnsubscribe = 0xA,
    MqttPacketTypeUnsubAck = 0xB,
    MqttPacketTypePingReq = 0xC,
    MqttPacketTypePingResp = 0xD,
    MqttPacketTypeDisconnect = 0xE
};

enum MqttPacketState
{
    MqttPacketStateReadType,
    MqttPacketStateReadRemainingLength,
    MqttPacketStateReadPayload,
    MqttPacketStateReadComplete,

    MqttPacketStateWriteType,
    MqttPacketStateWriteRemainingLength,
    MqttPacketStateWritePayload,
    MqttPacketStateWriteComplete
};

struct MqttMessage;

typedef struct MqttPacket MqttPacket;

struct MqttPacket
{
    int type;
    int flags;
    int state;
    uint16_t id;
    size_t remainingLength;
    bstring payload;
    struct MqttMessage *message;
    SIMPLEQ_ENTRY(MqttPacket) sendQueue;
};

const char *MqttPacketName(int type);

MqttPacket *MqttPacketNew(int type);

MqttPacket *MqttPacketWithIdNew(int type, uint16_t id);

void MqttPacketFree(MqttPacket *packet);

#endif
