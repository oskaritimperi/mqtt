#ifndef MQTT_H
#define MQTT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

typedef enum MqttConnectionStatus
{
    MqttConnectionAccepted = 0,
    MqttConnectionInvalidProtocolVersion,
    MqttConnectionIdentifierRejected,
    MqttConnectionServerUnavailable,
    MqttConnectionBadAuth,
    MqttConnectionNotAuthorized
} MqttConnectionStatus;

typedef enum MqttSubscriptionStatus
{
    MqttSubscriptionQos0 = 0,
    MqttSubscriptionQos1 = 1,
    MqttSubscriptionQos2 = 2,
    MqttSubscriptionFailure = 0x80
} MqttSubscriptionStatus;

typedef struct MqttClient MqttClient;

typedef void (*MqttClientOnConnectCallback)(MqttClient *client,
                                            MqttConnectionStatus status,
                                            int sessionPresent);

typedef void (*MqttClientOnSubscribeCallback)(MqttClient *client,
                                              int id,
                                              const char *topicFilter,
                                              MqttSubscriptionStatus status);

typedef void (*MqttClientOnUnsubscribeCallback)(MqttClient *client, int id);

typedef void (*MqttClientOnMessageCallback)(MqttClient *client,
                                            const char *topic,
                                            const void *data,
                                            size_t size,
                                            int qos,
                                            int retain);

typedef void (*MqttClientOnPublishCallback)(MqttClient *client, int id);

MqttClient *MqttClientNew(const char *clientId);

void MqttClientFree(MqttClient *client);

void MqttClientSetUserData(MqttClient *client, void *userData);

void *MqttClientGetUserData(MqttClient *client);

void MqttClientSetOnConnect(MqttClient *client, MqttClientOnConnectCallback cb);

void MqttClientSetOnSubscribe(MqttClient *client,
                              MqttClientOnSubscribeCallback cb);

void MqttClientSetOnUnsubscribe(MqttClient *client,
                                MqttClientOnUnsubscribeCallback cb);

void MqttClientSetOnMessage(MqttClient *client,
                            MqttClientOnMessageCallback cb);

void MqttClientSetOnPublish(MqttClient *client,
                            MqttClientOnPublishCallback cb);

int MqttClientConnect(MqttClient *client, const char *host, short port,
                      int keepAlive, int cleanSession);

int MqttClientDisconnect(MqttClient *client);

int MqttClientIsConnected(MqttClient *client);

int MqttClientRunOnce(MqttClient *client, int timeout);

int MqttClientRun(MqttClient *client);

int MqttClientSubscribe(MqttClient *client, const char *topicFilter,
                        int qos);

int MqttClientSubscribeMany(MqttClient *client, const char **topicFilters,
  int *qos, size_t count);

int MqttClientUnsubscribe(MqttClient *client, const char *topicFilter);

int MqttClientPublish(MqttClient *client, int qos, int retain,
                      const char *topic, const void *data, size_t size);

int MqttClientPublishCString(MqttClient *client, int qos, int retain,
                             const char *topic, const char *msg);

void MqttClientSetPublishRetryTimeout(MqttClient *client, int timeout);

void MqttClientSetMaxMessagesInflight(MqttClient *client, int max);

void MqttClientSetMaxQueuedMessages(MqttClient *client, int max);

int MqttClientSetWill(MqttClient *client, const char *topic, const void *msg,
                      size_t size, int qos, int retain);

#if defined(__cplusplus)
}
#endif

#endif
