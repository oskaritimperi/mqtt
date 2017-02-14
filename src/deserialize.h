#ifndef DESERIALIZE_H
#define DESERIALIZE_H

#include "config.h"

typedef struct MqttPacket MqttPacket;
typedef struct Stream Stream;

int MqttPacketDeserialize(MqttPacket **packet, Stream *stream);

#endif
