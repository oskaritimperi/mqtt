#ifndef PACKET_H
#define PACKET_H

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

typedef struct MqttPacket MqttPacket;

struct MqttPacket
{
    int type;
    uint16_t id;
    int state;
    int flags;
    int64_t sentAt;
    SIMPLEQ_ENTRY(MqttPacket) sendQueue;
    TAILQ_ENTRY(MqttPacket) messages;
};

#define MqttPacketType(packet) (((MqttPacket *) (packet))->type)

#define MqttPacketId(packet) (((MqttPacket *) (packet))->id)

#define MqttPacketSentAt(packet) (((MqttPacket *) (packet))->sentAt)

typedef struct MqttPacketConnect MqttPacketConnect;

struct MqttPacketConnect
{
    MqttPacket base;
    char connectFlags;
    uint16_t keepAlive;
    bstring clientId;
    bstring willTopic;
    bstring willMessage;
    bstring userName;
    bstring password;
};

typedef struct MqttPacketConnAck MqttPacketConnAck;

struct MqttPacketConnAck
{
    MqttPacket base;
    unsigned char connAckFlags;
    unsigned char returnCode;
};

typedef struct MqttPacketPublish MqttPacketPublish;

struct MqttPacketPublish
{
    MqttPacket base;
    bstring topicName;
    bstring message;
    char qos;
    char dup;
    char retain;
};

#define MqttPacketPublishQos(p) (((MqttPacketPublish *) p)->qos)
#define MqttPacketPublishDup(p) (((MqttPacketPublish *) p)->dup)
#define MqttPacketPublishRetain(p) (((MqttPacketPublish *) p)->retain)

typedef struct MqttPacketSubscribe MqttPacketSubscribe;

struct MqttPacketSubscribe
{
    MqttPacket base;
    bstring topicFilter;
    char qos;
};

typedef struct MqttPacketSubAck MqttPacketSubAck;

struct MqttPacketSubAck
{
    MqttPacket base;
    unsigned char returnCode;
};

typedef struct MqttPacketUnsubscribe MqttPacketUnsubscribe;

struct MqttPacketUnsubscribe
{
    MqttPacket base;
    bstring topicFilter;
};

const char *MqttPacketName(int type);

MqttPacket *MqttPacketNew(int type);

MqttPacket *MqttPacketWithIdNew(int type, uint16_t id);

void MqttPacketFree(MqttPacket *packet);

int MqttPacketHasId(const MqttPacket *packet);

#endif
