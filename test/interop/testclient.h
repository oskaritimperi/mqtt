#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include "mqtt.h"
#include "private.h"
#include "queue.h"

typedef struct Message Message;

struct Message
{
    SIMPLEQ_ENTRY(Message) chain;
    char *topic;
    void *data;
    size_t size;
    int qos;
    int retain;
};

typedef struct TestClient TestClient;

struct TestClient
{
    MqttClient *client;

    /* OnConnect */
    MqttConnectionStatus connectionStatus;
    int sessionPresent;

    /* OnSubscribe */
    int subId;
    MqttSubscriptionStatus subStatus[16];
    int subCount;

    /* OnPublish */
    int pubId;

    /* OnMessage */
    SIMPLEQ_HEAD(messages, Message) messages;
};

Message *MessageNew(const char *topic, const void *data, size_t size,
                    int qos, int retain);

void MessageFree(Message *msg);

TestClient *TestClientNew(const char *clientId);

void TestClientFree(TestClient *client);

int TestClientConnect(TestClient *client, const char *host, int port,
                      int keepAlive, int cleanSession);

void TestClientDisconnect(TestClient *client);

int TestClientSubscribe(TestClient *client, const char *topicFilter, int qos);

int TestClientSubscribeMany(TestClient *client, const char **topicFilter,
                            int *qos, size_t count);

int TestClientPublish(TestClient *client, int qos, int retain,
                      const char *topic, const char *message);

int TestClientMessageCount(TestClient *client);

int TestClientWait(TestClient *client, int timeout);

#endif
