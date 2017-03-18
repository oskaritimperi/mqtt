#include "message.h"
#include "stringstream.h"
#include "stream_mqtt.h"
#include "packet.h"

void MqttMessageFree(MqttMessage *msg)
{
    bdestroy(msg->topic);
    bdestroy(msg->payload);
    free(msg);
}
