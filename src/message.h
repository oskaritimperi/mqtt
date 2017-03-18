#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "queue.h"
#include <bstrlib/bstrlib.h>

enum MqttMessageState
{
    MqttMessageStateQueued,
    MqttMessageStatePublish,
    MqttMessageStateWaitPubAck,
    MqttMessageStateWaitPubRec,
    MqttMessageStateWaitPubComp,
    MqttMessageStateWaitPubRel
};

typedef struct MqttMessage MqttMessage;

struct MqttMessage
{
    int state;
    int qos;
    int retain;
    int dup;
    int padding;
    uint16_t id;
    int64_t timestamp;
    bstring topic;
    bstring payload;
    TAILQ_ENTRY(MqttMessage) chain;
};

typedef struct MqttMessageList MqttMessageList;
TAILQ_HEAD(MqttMessageList, MqttMessage);

void MqttMessageFree(MqttMessage *msg);

#endif
