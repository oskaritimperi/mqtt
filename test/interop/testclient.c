#include "testclient.h"
#include "misc.h"

#include <stdio.h>
#include <string.h>

static void TestClientOnConnect(MqttClient *client,
                                MqttConnectionStatus status,
                                int sessionPresent)
{
    TestClient *testClient = (TestClient *) MqttClientGetUserData(client);
    testClient->connectionStatus = status;
    testClient->sessionPresent = sessionPresent;
}

static void TestClientOnSubscribe(MqttClient *client, int id,
                                  const char *filter,
                                  MqttSubscriptionStatus status)
{
    TestClient *testClient = (TestClient *) MqttClientGetUserData(client);
    testClient->subId = id;
    testClient->subStatus[testClient->subCount++] = status;
}

static void TestClientOnPublish(MqttClient *client, int id)
{
    TestClient *testClient = (TestClient *) MqttClientGetUserData(client);
    testClient->pubId = id;
}

static void TestClientOnMessage(MqttClient *client, const char *topic,
                                const void *data, size_t size, int qos,
                                int retain)
{
    Message *msg = MessageNew(topic, data, size, qos, retain);
    TestClient *testClient = (TestClient *) MqttClientGetUserData(client);
    SIMPLEQ_INSERT_TAIL(&testClient->messages, msg, chain);
}

Message *MessageNew(const char *topic, const void *data, size_t size,
                    int qos, int retain)
{
    Message *msg = calloc(1, sizeof(*msg));
    int topicLen;

    topicLen = strlen(topic);
    msg->topic = (char *) malloc(topicLen+1);
    memcpy(msg->topic, topic, topicLen);
    msg->topic[topicLen] = '\0';

    msg->data = malloc(size);
    memcpy(msg->data, data, size);
    msg->size = size;

    msg->qos = qos;
    msg->retain = retain;

    return msg;
}

void MessageFree(Message *msg)
{
    free(msg->topic);
    free(msg->data);
    free(msg);
}

TestClient *TestClientNew(const char *clientId)
{
    TestClient *client = calloc(1, sizeof(*client));

    client->client = MqttClientNew(clientId);

    MqttClientSetUserData(client->client, client);

    client->connectionStatus = (MqttConnectionStatus) -1;

    MqttClientSetOnConnect(client->client, TestClientOnConnect);
    MqttClientSetOnSubscribe(client->client, TestClientOnSubscribe);
    MqttClientSetOnPublish(client->client, TestClientOnPublish);
    MqttClientSetOnMessage(client->client, TestClientOnMessage);

    SIMPLEQ_INIT(&client->messages);

    return client;
}

void TestClientFree(TestClient *client)
{
    MqttClientFree(client->client);

    while (!SIMPLEQ_EMPTY(&client->messages))
    {
        Message *msg = SIMPLEQ_FIRST(&client->messages);
        SIMPLEQ_REMOVE_HEAD(&client->messages, chain);
        MessageFree(msg);
    }

    free(client);
}

int TestClientConnect(TestClient *client, const char *host, int port,
                      int keepAlive, int cleanSession)
{
    MqttClientConnect(client->client, host, port, keepAlive, cleanSession);

    while (MqttClientRunOnce(client->client, -1) != -1)
    {
        if (client->connectionStatus != (MqttConnectionStatus) -1)
        {
            break;
        }
    }

    return client->connectionStatus == MqttConnectionAccepted;
}

void TestClientDisconnect(TestClient *client)
{
    MqttClientDisconnect(client->client);

    while (MqttClientRunOnce(client->client, -1) != -1)
    {
        if (!MqttClientIsConnected(client->client))
        {
            break;
        }
    }
}

int TestClientSubscribe(TestClient *client, const char *topicFilter, int qos)
{
    int id = MqttClientSubscribe(client->client, topicFilter, qos);

    client->subId = -1;
    client->subCount = 0;

    while (MqttClientRunOnce(client->client, -1) != -1)
    {
        if (client->subId != -1)
        {
            if (client->subId != id)
            {
                printf(
                    "WARNING: subscription id mismatch: expected %d, got %d\n",
                    id, client->subId);
            }
            break;
        }
    }

    return client->subStatus[0] != MqttSubscriptionFailure;
}

int TestClientSubscribeMany(TestClient *client, const char **topicFilter,
                            int *qos, size_t count)
{
    int id = MqttClientSubscribeMany(client->client, topicFilter, qos, count);
    int fail = 0, i;

    client->subId = -1;
    client->subCount = 0;

    while (MqttClientRunOnce(client->client, -1) != -1)
    {
        if (client->subId != -1)
        {
            if (client->subId != id)
            {
                printf(
                    "WARNING: subscription id mismatch: expected %d, got %d\n",
                    id, client->subId);
            }
            break;
        }
    }

    for (i = 0; i < client->subCount; ++i)
    {
        if (client->subStatus[i] == MqttSubscriptionFailure)
        {
            fail = 1;
            break;
        }
    }

    return !fail;
}

int TestClientPublish(TestClient *client, int qos, int retain,
                      const char *topic, const char *message)
{
    int id = MqttClientPublishCString(client->client, qos, retain, topic,
                                      message);

    client->pubId = -1;

    while (MqttClientRunOnce(client->client, -1) != -1)
    {
        if (qos == 0)
            return 1;

        if (client->pubId != -1)
        {
            if (client->pubId != id)
            {
                printf("WARNING: publish id mismatch: expected %d, got %d",
                       id, client->pubId);
            }
            break;
        }
    }

    return client->pubId == id;
}

int TestClientMessageCount(TestClient *client)
{
    int count = 0;
    Message *msg;
    SIMPLEQ_FOREACH(msg, &client->messages, chain)
    {
        ++count;
    }
    return count;
}

int TestClientWait(TestClient *client, int timeout)
{
    int64_t start = MqttGetCurrentTime();
    int rc;

    while ((rc = MqttClientRunOnce(client->client, timeout)) != -1)
    {
        printf("TestClientWait timeout:%d rc:%d\n", timeout, rc);
        int64_t now = MqttGetCurrentTime();
        int64_t elapsed = now - start;
        timeout -= elapsed;
        printf("TestClientWait elapsed:%d\n", (int) elapsed);
        if (timeout <= 0)
        {
            break;
        }
    }

    return rc != -1;
}
