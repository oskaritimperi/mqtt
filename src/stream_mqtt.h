#ifndef STREAM_MQTT_H
#define STREAM_MQTT_H

#include "stream.h"
#include "stringstream.h"

#include <bstrlib/bstrlib.h>

int64_t StreamReadMqttString(bstring *buf, Stream *stream);
int64_t StreamWriteMqttString(const_bstring buf, Stream *stream);

int64_t StreamReadRemainingLength(size_t *remainingLength, Stream *stream);
int64_t StreamWriteRemainingLength(size_t remainingLength, Stream *stream);

#endif
