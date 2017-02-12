#ifndef SERIALIZE_H
#define SERIALIZE_H

typedef struct MqttPacket MqttPacket;
typedef struct Stream Stream;

int MqttPacketSerialize(const MqttPacket *packet, Stream *stream);

#endif
