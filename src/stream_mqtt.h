#ifndef STREAM_MQTT_H
#define STREAM_MQTT_H

#include "stream.h"

int64_t StreamReadMqttString(char **s, size_t *len, Stream *stream);
int64_t StreamWriteMqttString(const char *s, int len, Stream *stream);

struct StringBuf;

int64_t StreamReadMqttStringBuf(struct StringBuf *buf, Stream *stream);
int64_t StreamWriteMqttStringBuf(const struct StringBuf *buf, Stream *stream);

int64_t StreamReadRemainingLength(size_t *remainingLength, Stream *stream);
int64_t StreamWriteRemainingLength(size_t remainingLength, Stream *stream);

#endif
