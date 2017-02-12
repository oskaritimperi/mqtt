#ifndef DESERIALIZE_H
#define DESERIALIZE_H

typedef struct MqttPacket MqttPacket;
typedef struct Stream Stream;

int MqttPacketDeserialize(MqttPacket **packet, Stream *stream);

#endif
