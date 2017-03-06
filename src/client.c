#include "config.h"
#include "mqtt.h"
#include "packet.h"
#include "stream.h"
#include "socketstream.h"
#include "socket.h"
#include "misc.h"
#include "log.h"
#include "private.h"
#include "stringstream.h"
#include "stream_mqtt.h"
#include "message.h"

#include "queue.h"

#include <bstrlib/bstrlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>

#if (LOG_LEVEL == LOG_LEVEL_DEBUG) && !defined(PRId64)
#error define PRId64 for your platform
#endif

struct MqttClient
{
    SocketStream stream;
    /* client id, NULL if we want to have server generated id */
    bstring clientId;
    /* set to 1 if we want to have a clean session */
    int cleanSession;
    /* remote host and port */
    bstring host;
    short port;
    /* keepalive interval in seconds */
    int keepAlive;
    /* user specified data, not used by us */
    void *userData;
    /* callback called after connection is made */
    MqttClientOnConnectCallback onConnect;
    /* callback called after subscribe is done */
    MqttClientOnSubscribeCallback onSubscribe;
    /* callback called after subscribe is done */
    MqttClientOnUnsubscribeCallback onUnsubscribe;
    /* callback called when a message is received */
    MqttClientOnMessageCallback onMessage;
    /* callback called after publish is done and acknowledged */
    MqttClientOnPublishCallback onPublish;
    int stopped;
    /* packets waiting to be sent over network */
    SIMPLEQ_HEAD(, MqttPacket) sendQueue;
    /* sent messages that are not done yet */
    MqttMessageList outMessages;
    /* received messages that are not done yet  */
    MqttMessageList inMessages;
    int sessionPresent;
    /* when was the last packet sent */
    int64_t lastPacketSentTime;
    /* next packet id */
    uint16_t packetId;
    /* timeout after which to retry sending messages */
    int retryTimeout;
    /* maximum number of inflight messages (not packets!) */
    int maxInflight;
    /* maximum number of queued messages (not packets!) */
    int maxQueued;
    /* 1 if PINGREQ is sent and we are waiting for PINGRESP, 0 otherwise */
    int pingSent;
    bstring willTopic;
    bstring willMessage;
    int willQos;
    int willRetain;
    /* 1 if client should ignore incoming PUBLISH messages, 0 handle them */
    int paused;
    bstring userName;
    bstring password;
    /* The packet we are receiving */
    MqttPacket inPacket;
};

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet);
static int MqttClientQueueSimplePacket(MqttClient *client, int type);
static int MqttClientSendPacket(MqttClient *client);
static int MqttClientRecvPacket(MqttClient *client);
static uint16_t MqttClientNextPacketId(MqttClient *client);
static void MqttClientProcessMessageQueue(MqttClient *client);
static void MqttClientClearQueues(MqttClient *client);

static MQTT_INLINE int MqttClientInflightMessageCount(MqttClient *client)
{
    MqttMessage *msg;
    int queued = 0;
    int inMessagesCount = 0;
    int outMessagesCount = 0;

    TAILQ_FOREACH(msg, &client->outMessages, chain)
    {
        if (msg->state == MqttMessageStateQueued)
        {
            ++queued;
        }

        ++outMessagesCount;
    }

    TAILQ_FOREACH(msg, &client->inMessages, chain)
    {
        ++inMessagesCount;
    }

    return inMessagesCount + outMessagesCount - queued;
}

MqttClient *MqttClientNew(const char *clientId)
{
    MqttClient *client;

    client = calloc(1, sizeof(*client));

    if (!client)
    {
        return NULL;
    }

    client->clientId = bfromcstr(clientId);

    client->stream.sock = -1;

    client->retryTimeout = 20;

    client->maxQueued = 0;
    client->maxInflight = 20;

    TAILQ_INIT(&client->outMessages);
    TAILQ_INIT(&client->inMessages);
    SIMPLEQ_INIT(&client->sendQueue);

    return client;
}

void MqttClientFree(MqttClient *client)
{
    MqttClientClearQueues(client);

    bdestroy(client->clientId);
    bdestroy(client->willTopic);
    bdestroy(client->willMessage);
    bdestroy(client->host);
    bdestroy(client->userName);
    bdestroy(client->password);

    if (client->stream.sock != -1)
    {
        SocketDisconnect(client->stream.sock);
    }

    free(client);
}

void MqttClientSetUserData(MqttClient *client, void *userData)
{
    assert(client != NULL);
    client->userData = userData;
}

void *MqttClientGetUserData(MqttClient *client)
{
    assert(client != NULL);
    return client->userData;
}

void MqttClientSetOnConnect(MqttClient *client, MqttClientOnConnectCallback cb)
{
    assert(client != NULL);
    client->onConnect = cb;
}

void MqttClientSetOnSubscribe(MqttClient *client,
                              MqttClientOnSubscribeCallback cb)
{
    assert(client != NULL);
    client->onSubscribe = cb;
}

void MqttClientSetOnUnsubscribe(MqttClient *client,
                                MqttClientOnUnsubscribeCallback cb)
{
    assert(client != NULL);
    client->onUnsubscribe = cb;
}

void MqttClientSetOnMessage(MqttClient *client,
                            MqttClientOnMessageCallback cb)
{
    assert(client != NULL);
    client->onMessage = cb;
}

void MqttClientSetOnPublish(MqttClient *client,
                            MqttClientOnPublishCallback cb)
{
    assert(client != NULL);
    client->onPublish = cb;
}

static const struct tagbstring MqttProtocolId = bsStatic("MQTT");
static const char MqttProtocolLevel  = 0x04;

static unsigned char MqttClientConnectFlags(MqttClient *client)
{
    unsigned char connectFlags = 0;

    if (client->cleanSession)
    {
        connectFlags |= 0x02;
    }

    if (client->willTopic)
    {
        connectFlags |= 0x04;
        connectFlags |= (client->willQos & 3) << 3;
        connectFlags |= (client->willRetain & 1) << 5;
    }

    if (client->userName)
    {
        connectFlags |= 0x80;
        if (client->password)
        {
            connectFlags |= 0x40;
        }
    }

    return connectFlags;
}

int MqttClientConnect(MqttClient *client, const char *host, short port,
                      int keepAlive, int cleanSession)
{
    int sock;
    MqttPacket *packet;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    assert(client != NULL);
    assert(host != NULL);

    if (client->host)
        bassigncstr(client->host, host);
    else
        client->host = bfromcstr(host);
    client->port = port;
    client->keepAlive = keepAlive;
    client->cleanSession = cleanSession;

    /* In case we are reconnecting */
    client->stopped = 0;
    client->pingSent = 0;
    MqttClientClearQueues(client);

    if (keepAlive < 0)
    {
        LOG_ERROR("invalid keepAlive: %d", keepAlive);
        return -1;
    }

    LOG_DEBUG("connecting");

    if ((sock = SocketConnect(host, port)) == -1)
    {
        LOG_ERROR("SocketConnect failed!");
        return -1;
    }

    if (SocketStreamOpen(&client->stream, sock) == -1)
    {
        return -1;
    }

    packet = MqttPacketNew(MqttPacketTypeConnect);

    if (!packet)
        return -1;

    StringStreamInit(&ss);

    StreamWriteMqttString(&MqttProtocolId, pss);
    StreamWriteByte(MqttProtocolLevel, pss);
    StreamWriteByte(MqttClientConnectFlags(client), pss);
    StreamWriteUint16Be(client->keepAlive, pss);
    StreamWriteMqttString(client->clientId, pss);

    if (client->willTopic)
    {
        StreamWriteMqttString(client->willTopic, pss);
        StreamWriteMqttString(client->willMessage, pss);
    }

    if (client->userName)
    {
        StreamWriteMqttString(client->userName, pss);
        if(client->password)
        {
            StreamWriteMqttString(client->password, pss);
        }
    }

    packet->payload = ss.buffer;

    MqttClientQueuePacket(client, packet);

    return 0;
}

int MqttClientDisconnect(MqttClient *client)
{
    LOG_DEBUG("disconnecting");
    return MqttClientQueueSimplePacket(client, MqttPacketTypeDisconnect);
}

int MqttClientIsConnected(MqttClient *client)
{
    return client->stream.sock != -1;
}

int MqttClientRunOnce(MqttClient *client, int timeout)
{
    int rv;
    int events;

    assert(client != NULL);

    if (client->stream.sock == -1)
    {
        LOG_ERROR("invalid socket");
        return -1;
    }

    events = EV_READ;

    /* Handle outMessages and inMessages, moving queued messages to sendQueue
       if there are less than maxInflight number of messages in flight */
    MqttClientProcessMessageQueue(client);

    if (SIMPLEQ_EMPTY(&client->sendQueue))
    {
        LOG_DEBUG("nothing to write");
    }
    else
    {
        events |= EV_WRITE;
    }

    LOG_DEBUG("selecting");

    if (timeout < 0)
    {
        timeout = client->keepAlive * 1000;
        if (timeout == 0)
        {
            timeout = 30 * 1000;
        }
    }
    else if (timeout > (client->keepAlive * 1000) && client->keepAlive > 0)
    {
        timeout = client->keepAlive * 1000;
    }

    rv = SocketSelect(client->stream.sock, &events, timeout);

    if (rv == -1)
    {
        LOG_ERROR("select failed");
        return -1;
    }
    else if (rv)
    {
        LOG_DEBUG("select rv=%d", rv);

        if (events & EV_WRITE)
        {
            LOG_DEBUG("socket writable");

            if (MqttClientSendPacket(client) == -1)
            {
                LOG_ERROR("MqttClientSendPacket failed");
                client->stopped = 1;
            }
        }

        if (events & EV_READ)
        {
            LOG_DEBUG("socket readable");

            if (MqttClientRecvPacket(client) == -1)
            {
                LOG_ERROR("MqttClientRecvPacket failed");
                client->stopped = 1;
            }
        }
    }
    else
    {
        LOG_DEBUG("select timeout");

        if (client->pingSent)
        {
            LOG_ERROR("no PINGRESP received in time");
            client->pingSent = 0;
            client->stopped = 1;
        }
        else if (SIMPLEQ_EMPTY(&client->sendQueue))
        {
            int64_t elapsed = MqttGetCurrentTime() - client->lastPacketSentTime;
            if (elapsed/1000 >= client->keepAlive && client->keepAlive > 0)
            {
                MqttClientQueueSimplePacket(client, MqttPacketTypePingReq);
                client->pingSent = 1;
            }
        }
    }

    if (client->stopped)
    {
        SocketDisconnect(client->stream.sock);
        client->stream.sock = -1;
    }

    return 0;
}

int MqttClientRun(MqttClient *client)
{
    assert(client != NULL);

    while (!client->stopped)
    {
        if (MqttClientRunOnce(client, -1) == -1)
            return -1;
    }

    return 0;
}

int MqttClientSubscribe(MqttClient *client, const char *topicFilter,
                        int qos)
{
    return MqttClientSubscribeMany(client, &topicFilter, &qos, 1);
}

int MqttClientSubscribeMany(MqttClient *client, const char **topicFilters,
                            int *qos, size_t count)
{
    MqttPacket *packet = NULL;
    size_t i;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    assert(client != NULL);
    assert(topicFilters != NULL);
    assert(*topicFilters != NULL);
    assert(qos != NULL);
    assert(count > 0);

    packet = MqttPacketWithIdNew(MqttPacketTypeSubscribe,
                                 MqttClientNextPacketId(client));

    if (!packet)
        return -1;

    packet->flags = 0x2;

    StringStreamInit(&ss);

    StreamWriteUint16Be(packet->id, pss);

    LOG_DEBUG("SUBSCRIBE id:%d", (int) packet->id);

    for (i = 0; i < count; ++i)
    {
        struct tagbstring filter;
        btfromcstr(filter, topicFilters[i]);
        StreamWriteMqttString(&filter, pss);
        StreamWriteByte(qos[i] & 3, pss);
    }

    packet->payload = ss.buffer;

    MqttClientQueuePacket(client, packet);

    return packet->id;
}

int MqttClientUnsubscribe(MqttClient *client, const char *topicFilter)
{
    MqttPacket *packet = NULL;
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    struct tagbstring filter;

    assert(client != NULL);
    assert(topicFilter != NULL);

    packet = MqttPacketWithIdNew(MqttPacketTypeUnsubscribe,
                                 MqttClientNextPacketId(client));

    if (!packet)
        return -1;

    packet->flags = 0x02;

    StringStreamInit(&ss);

    StreamWriteUint16Be(packet->id, pss);

    btfromcstr(filter, topicFilter);

    StreamWriteMqttString(&filter, pss);

    packet->payload = ss.buffer;

    MqttClientQueuePacket(client, packet);

    return packet->id;
}

static MQTT_INLINE int MqttClientOutMessagesLen(MqttClient *client)
{
    MqttMessage *msg;
    int count = 0;
    TAILQ_FOREACH(msg, &client->outMessages, chain)
    {
        ++count;
    }
    return count;
}

static MqttPacket *PublishToPacket(MqttMessage *msg)
{
    MqttPacket *packet = NULL;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    if (msg->qos > 0)
    {
        packet = MqttPacketWithIdNew(MqttPacketTypePublish,
                                     msg->id);
    }
    else
    {
        packet = MqttPacketNew(MqttPacketTypePublish);
    }

    if (!packet)
        return NULL;

    packet->message = msg;

    StringStreamInit(&ss);

    StreamWriteMqttString(msg->topic, pss);

    if (msg->qos > 0)
    {
        StreamWriteUint16Be(msg->id, pss);
    }

    StreamWrite(bdata(msg->payload), blength(msg->payload), pss);

    packet->payload = ss.buffer;
    packet->flags = (msg->qos & 3) << 1;
    packet->flags |= msg->retain & 1;

    return packet;
}

int MqttClientPublish(MqttClient *client, int qos, int retain,
                      const char *topic, const void *data, size_t size)
{
    MqttMessage *message;

    /* first check if the queue is already full */
    if (qos > 0 && client->maxQueued > 0 &&
        MqttClientOutMessagesLen(client) >= client->maxQueued)
    {
        return -1;
    }

    message = calloc(1, sizeof(*message));
    if (!message)
    {
        return -1;
    }

    message->state = MqttMessageStateQueued;
    message->qos = qos;
    message->retain = retain;
    message->dup = 0;
    message->timestamp = MqttGetCurrentTime();

    if (qos == 0)
    {
        /* Copy payload and topic directly from user buffers as we don't need
           to keep the message data around after this function. */
        MqttPacket *packet;
        struct tagbstring bttopic, btpayload;

        btfromcstr(bttopic, topic);
        message->topic = &bttopic;

        btfromblk(btpayload, data, size);
        message->payload = &btpayload;

        packet = PublishToPacket(message);

        message->topic = NULL;
        message->payload = NULL;

        MqttClientQueuePacket(client, packet);

        MqttMessageFree(message);

        return 0;
    }
    else
    {
        /* Duplicate the user buffers as we need the data to be available
           longer. */
        message->topic = bfromcstr(topic);
        message->payload = blk2bstr(data, size);

        message->id = MqttClientNextPacketId(client);

        TAILQ_INSERT_TAIL(&client->outMessages, message, chain);

        return message->id;
    }
}

int MqttClientPublishCString(MqttClient *client, int qos, int retain,
                             const char *topic, const char *msg)
{
    return MqttClientPublish(client, qos, retain, topic, msg, strlen(msg));
}

void MqttClientSetPublishRetryTimeout(MqttClient *client, int timeout)
{
    assert(client != NULL);
    client->retryTimeout = timeout;
}

void MqttClientSetMaxMessagesInflight(MqttClient *client, int max)
{
    assert(client != NULL);
    client->maxInflight = max;
}

void MqttClientSetMaxQueuedMessages(MqttClient *client, int max)
{
    assert(client != NULL);
    client->maxQueued = max;
}

int MqttClientSetWill(MqttClient *client, const char *topic, const void *msg,
                      size_t size, int qos, int retain)
{
    assert(client != NULL);

    if (client->stream.sock != -1)
    {
        LOG_ERROR("MqttClientSetWill must be called before MqttClientConnect");
        return -1;
    }

    client->willTopic = bfromcstr(topic);
    client->willMessage = blk2bstr(msg, size);
    client->willQos = qos;
    client->willRetain = retain;

    return 0;
}

int MqttClientSetAuth(MqttClient *client, const char *userName,
                      const char *password)
{
    assert(client != NULL);

    if (MqttClientIsConnected(client))
    {
        LOG_ERROR("MqttClientSetAuth must be called before MqttClientConnect");
        return -1;
    }

    if (userName)
    {
        if (client->userName)
            bassigncstr(client->userName, userName);
        else
            client->userName = bfromcstr(userName);

        if (password)
        {
            if (client->password)
                bassigncstr(client->password, password);
            else
                client->password = bfromcstr(password);
        }
        else
        {
            bdestroy(client->password);
            client->password = NULL;
        }
    }
    else
    {
        bdestroy(client->userName);
        client->userName = NULL;

        bdestroy(client->password);
        client->password = NULL;
    }

    return 0;
}

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet)
{
    assert(client != NULL);
    LOG_DEBUG("queuing packet %s", MqttPacketName(packet->type));
    packet->state = MqttPacketStateWriteType;
    SIMPLEQ_INSERT_TAIL(&client->sendQueue, packet, sendQueue);
}

static int MqttClientQueueSimplePacket(MqttClient *client, int type)
{
    MqttPacket *packet = MqttPacketNew(type);
    if (!packet)
        return -1;
    MqttClientQueuePacket(client, packet);
    return 0;
}

static int MqttClientSendPacket(MqttClient *client)
{
    MqttPacket *packet;

    packet = SIMPLEQ_FIRST(&client->sendQueue);

    if (!packet)
    {
        LOG_WARNING("MqttClientSendPacket called with no queued packets");
        return 0;
    }

    while (packet != NULL)
    {
        switch (packet->state)
        {
            case MqttPacketStateWriteType:
            {
                unsigned char typeAndFlags = ((packet->type & 0x0F) << 4) |
                                             (packet->flags & 0x0F);

                if (StreamWriteByte(typeAndFlags, &client->stream.base) == -1)
                {
                    return -1;
                }

                packet->state = MqttPacketStateWriteRemainingLength;

                break;
            }

            case MqttPacketStateWriteRemainingLength:
            {
                if (StreamWriteRemainingLength(blength(packet->payload),
                                               &client->stream.base) == -1)
                {
                    return -1;
                }

                packet->state = MqttPacketStateWritePayload;

                break;
            }

            case MqttPacketStateWritePayload:
            {
                if (packet->payload)
                {
                    if (StreamWrite(bdata(packet->payload),
                                    blength(packet->payload),
                                    &client->stream.base) == -1)
                    {
                        return -1;
                    }
                }

                packet->state = MqttPacketStateWriteComplete;

                break;
            }

            case MqttPacketStateWriteComplete:
            {
                client->lastPacketSentTime = MqttGetCurrentTime();

                if (packet->type == MqttPacketTypeDisconnect)
                {
                    client->stopped = 1;
                }

                LOG_DEBUG("sent %s", MqttPacketName(packet->type));

                if (packet->type == MqttPacketTypePublish && packet->message)
                {
                    MqttMessage *msg = packet->message;

                    if (msg->qos == 1)
                    {
                        msg->state = MqttMessageStateWaitPubAck;
                    }
                    else if (msg->qos == 2)
                    {
                        msg->state = MqttMessageStateWaitPubRec;
                    }
                }

                if (packet->message)
                {
                    packet->message->timestamp = client->lastPacketSentTime;
                }

                SIMPLEQ_REMOVE_HEAD(&client->sendQueue, sendQueue);

                MqttPacketFree(packet);

                packet = SIMPLEQ_FIRST(&client->sendQueue);

                break;
            }
        }
    }

    return 0;
}

static int MqttClientHandleConnAck(MqttClient *client)
{
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    unsigned char flags;
    unsigned char rc;

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadByte(&flags, pss);

    StreamReadByte(&rc, pss);

    client->sessionPresent = flags & 1;

    LOG_DEBUG("sessionPresent:%d", client->sessionPresent);

    if (client->onConnect)
    {
        LOG_DEBUG("calling onConnect rc:%d", rc);
        client->onConnect(client, rc, client->sessionPresent);
    }

    return 0;
}

static int MqttClientHandlePingResp(MqttClient *client)
{
    LOG_DEBUG("got ping response");
    client->pingSent = 0;
    return 0;
}

static int MqttClientHandleSubAck(MqttClient *client)
{
    uint16_t id;
    int *qos;
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    int count;
    int i;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    LOG_DEBUG("received SUBACK with id:%d", (int) id);

    count = blength(client->inPacket.payload) - StreamTell(pss);

    if (count <= 0)
    {
        LOG_ERROR("number of return codes invalid");
        return -1;
    }

    qos = malloc(count * sizeof(int));

    for (i = 0; i < count; ++i)
    {
        unsigned char byte;
        StreamReadByte(&byte, pss);
        qos[i] = byte;
    }

    if (client->onSubscribe)
    {
        client->onSubscribe(client, id, qos, count);
    }

    free(qos);

    return 0;
}

static int MqttClientSendPubAck(MqttClient *client, uint16_t id)
{
    MqttPacket *packet;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    packet = MqttPacketWithIdNew(MqttPacketTypePubAck, id);

    if (!packet)
        return -1;

    StringStreamInit(&ss);

    StreamWriteUint16Be(id, pss);

    packet->payload = ss.buffer;

    MqttClientQueuePacket(client, packet);

    return 0;
}

static int MqttClientSendPubRec(MqttClient *client, MqttMessage *msg)
{
    MqttPacket *packet;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    packet = MqttPacketWithIdNew(MqttPacketTypePubRec, msg->id);

    if (!packet)
        return -1;

    StringStreamInit(&ss);

    StreamWriteUint16Be(msg->id, pss);

    packet->payload = ss.buffer;
    packet->message = msg;

    MqttClientQueuePacket(client, packet);

    return 0;
}

static int MqttClientSendPubRel(MqttClient *client, MqttMessage *msg)
{
    MqttPacket *packet;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    packet = MqttPacketWithIdNew(MqttPacketTypePubRel, msg->id);

    if (!packet)
        return -1;

    packet->flags = 0x2;

    StringStreamInit(&ss);

    StreamWriteUint16Be(msg->id, pss);

    packet->payload = ss.buffer;
    packet->message = msg;

    MqttClientQueuePacket(client, packet);

    return 0;
}

static int MqttClientSendPubComp(MqttClient *client, uint16_t id)
{
    MqttPacket *packet;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    packet = MqttPacketWithIdNew(MqttPacketTypePubComp, id);

    if (!packet)
        return -1;

    StringStreamInit(&ss);

    StreamWriteUint16Be(id, pss);

    packet->payload = ss.buffer;

    MqttClientQueuePacket(client, packet);

    return 0;
}

static int MqttClientHandlePublish(MqttClient *client)
{
    MqttMessage *msg;
    uint16_t id;
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    MqttPacket *packet;
    int qos;
    int retain;
    bstring topic;
    void *payload;
    int payloadSize;

    /* We are paused - do nothing */
    if (client->paused)
        return 0;

    packet = &client->inPacket;

    qos = (packet->flags >> 1) & 3;
    retain = packet->flags & 1;

    StringStreamInitFromBstring(&ss, packet->payload);

    StreamReadMqttString(&topic, pss);

    StreamReadUint16Be(&id, pss);

    payload = bdataofs(ss.buffer, ss.pos);
    payloadSize = blength(ss.buffer) - ss.pos;

    if (qos == 2)
    {
        /* Check if we have sent a PUBREC previously with the same id. If we
           have, we have to resend the PUBREC. We must not call the onMessage
           callback again. */

        TAILQ_FOREACH(msg, &client->inMessages, chain)
        {
            if (msg->id == id &&
                msg->state == MqttMessageStateWaitPubRel)
            {
                break;
            }
        }

        if (msg)
        {
            LOG_DEBUG("resending PUBREC id:%u", msg->id);
            MqttClientSendPubRec(client, msg);
            bdestroy(topic);
            return 0;
        }
    }

    if (client->onMessage)
    {
        LOG_DEBUG("calling onMessage");
        client->onMessage(client,
            bdata(topic),
            payload,
            payloadSize,
            qos,
            retain);
    }

    bdestroy(topic);

    if (qos == 1)
    {
        MqttClientSendPubAck(client, id);
    }
    else if (qos == 2)
    {
        msg = calloc(1, sizeof(*msg));

        msg->state = MqttMessageStateWaitPubRel;
        msg->id = id;
        msg->qos = qos;

        TAILQ_INSERT_TAIL(&client->inMessages, msg, chain);

        MqttClientSendPubRec(client, msg);
    }

    return 0;
}

static int MqttClientHandlePubAck(MqttClient *client)
{
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    uint16_t id;
    MqttMessage *msg;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    TAILQ_FOREACH(msg, &client->outMessages, chain)
    {
        if (msg->id == id &&
            msg->state == MqttMessageStateWaitPubAck)
        {
            break;
        }
    }

    if (!msg)
    {
        LOG_ERROR("no message found with id %d", (int) id);
        return -1;
    }

    TAILQ_REMOVE(&client->outMessages, msg, chain);

    if (client->onPublish)
    {
        client->onPublish(client, msg->id);
    }

    MqttMessageFree(msg);

    return 0;
}

static int MqttClientHandlePubRec(MqttClient *client)
{
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    uint16_t id;
    MqttMessage *msg;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    TAILQ_FOREACH(msg, &client->outMessages, chain)
    {
        /* Also check if we are waiting for PUBCOMP, if we have sent PUBREL but
           they haven't received it.  */
        if (msg->id == id &&
            (msg->state == MqttMessageStateWaitPubRec ||
             msg->state == MqttMessageStateWaitPubComp))
        {
            break;
        }
    }

    if (!msg)
    {
        LOG_ERROR("no message found with id %d", (int) id);
        return -1;
    }

    msg->state = MqttMessageStateWaitPubComp;

    bdestroy(msg->payload);
    msg->payload = NULL;

    bdestroy(msg->topic);
    msg->topic = NULL;

    if (MqttClientSendPubRel(client, msg) == -1)
        return -1;

    return 0;
}

static int MqttClientHandlePubRel(MqttClient *client)
{
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    uint16_t id;
    MqttMessage *msg;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    TAILQ_FOREACH(msg, &client->inMessages, chain)
    {
        if (msg->id == id &&
            msg->state == MqttMessageStateWaitPubRel)
        {
            break;
        }
    }

    if (!msg)
    {
        LOG_ERROR("no message found with id %d", (int) id);
        return -1;
    }

    TAILQ_REMOVE(&client->inMessages, msg, chain);
    MqttMessageFree(msg);

    if (MqttClientSendPubComp(client, id) == -1)
        return -1;

    return 0;
}

static int MqttClientHandlePubComp(MqttClient *client)
{
    StringStream ss;
    Stream *pss = (Stream *) &ss;
    uint16_t id;
    MqttMessage *msg;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    TAILQ_FOREACH(msg, &client->outMessages, chain)
    {
        if (msg->id == id && msg->state == MqttMessageStateWaitPubComp)
        {
            break;
        }
    }

    if (!msg)
    {
        LOG_WARNING("no message found with id %d", (int) id);
        return 0;
    }

    TAILQ_REMOVE(&client->outMessages, msg, chain);

    MqttMessageFree(msg);

    if (client->onPublish)
    {
        LOG_DEBUG("calling onPublish id:%d", id);
        client->onPublish(client, id);
    }

    return 0;
}

static int MqttClientHandleUnsubAck(MqttClient *client)
{
    uint16_t id;
    StringStream ss;
    Stream *pss = (Stream *) &ss;

    assert(client != NULL);

    StringStreamInitFromBstring(&ss, client->inPacket.payload);

    StreamReadUint16Be(&id, pss);

    if (client->onUnsubscribe)
    {
        client->onUnsubscribe(client, id);
    }

    return 0;
}

static int MqttClientHandlePacket(MqttClient *client)
{
    int rc;

    switch (client->inPacket.type)
    {
        case MqttPacketTypeConnAck:
            rc = MqttClientHandleConnAck(client);
            break;

        case MqttPacketTypePingResp:
            rc = MqttClientHandlePingResp(client);
            break;

        case MqttPacketTypeSubAck:
            rc = MqttClientHandleSubAck(client);
            break;

        case MqttPacketTypeUnsubAck:
            rc = MqttClientHandleUnsubAck(client);
            break;

        case MqttPacketTypePubAck:
            rc = MqttClientHandlePubAck(client);
            break;

        case MqttPacketTypePubRec:
            rc = MqttClientHandlePubRec(client);
            break;

        case MqttPacketTypePubComp:
            rc = MqttClientHandlePubComp(client);
            break;

        case MqttPacketTypePubRel:
            rc = MqttClientHandlePubRel(client);
            break;

        case MqttPacketTypePublish:
            rc = MqttClientHandlePublish(client);
            break;

        default:
            LOG_ERROR("packet not handled yet");
            rc = -1;
            break;
    }

    bdestroy(client->inPacket.payload);
    client->inPacket.payload = NULL;

    client->inPacket.state = MqttPacketStateReadType;

    return rc;
}

static int MqttClientRecvPacket(MqttClient *client)
{
    while (1)
    {
        switch (client->inPacket.state)
        {
            case MqttPacketStateReadType:
            {
                unsigned char typeAndFlags;
                int rc;

                if ((rc = StreamReadByte(&typeAndFlags, &client->stream.base)) != 1)
                {
                    LOG_ERROR("failed reading packet type: %d", rc);
                    return -1;
                }

                client->inPacket.type = typeAndFlags >> 4;
                client->inPacket.flags = typeAndFlags & 0x0F;

                if (client->inPacket.type < MqttPacketTypeConnect ||
                    client->inPacket.type > MqttPacketTypeDisconnect)
                {
                    LOG_ERROR("unknown packet type: %d", client->inPacket.type);
                    return -1;
                }

                client->inPacket.state = MqttPacketStateReadRemainingLength;

                break;
            }

            case MqttPacketStateReadRemainingLength:
            {
                if (StreamReadRemainingLength(&client->inPacket.remainingLength,
                                              &client->stream.base) == -1)
                {
                    LOG_ERROR("failed to read remaining length");
                    return -1;
                }
                client->inPacket.state = MqttPacketStateReadPayload;
                break;
            }

            case MqttPacketStateReadPayload:
            {
                if (client->inPacket.remainingLength > 0)
                {
                    client->inPacket.payload = bfromcstr("");
                    ballocmin(client->inPacket.payload,
                              client->inPacket.remainingLength+1);
                    if (StreamRead(bdata(client->inPacket.payload),
                                   client->inPacket.remainingLength,
                                   &client->stream.base) == -1)
                    {
                        LOG_ERROR("failed reading packet payload");
                        bdestroy(client->inPacket.payload);
                        client->inPacket.payload = NULL;
                        return -1;
                    }
                    client->inPacket.payload->slen = client->inPacket.remainingLength;
                }
                client->inPacket.state = MqttPacketStateReadComplete;
                break;
            }

            case MqttPacketStateReadComplete:
            {
                int type = client->inPacket.type;
                LOG_DEBUG("received %s", MqttPacketName(type));
                return MqttClientHandlePacket(client);
            }
        }
    }

    return 0;
}

static uint16_t MqttClientNextPacketId(MqttClient *client)
{
    uint16_t id;
    assert(client != NULL);
    id = ++client->packetId;
    if (id == 0)
        id = ++client->packetId;
    return id;
}

static int64_t MqttMessageTimeSinceSent(MqttMessage *msg)
{
    int64_t now = MqttGetCurrentTime();
    return now - msg->timestamp;
}

static int MqttMessageShouldResend(MqttClient *client, MqttMessage *msg)
{
    if (msg->timestamp > 0 &&
        MqttMessageTimeSinceSent(msg) >= client->retryTimeout*1000)
    {
        return 1;
    }

    return 0;
}

static void MqttClientProcessInMessages(MqttClient *client)
{
    MqttMessage *msg, *next;

    TAILQ_FOREACH_SAFE(msg, &client->inMessages, chain, next)
    {
        switch (msg->state)
        {
            case MqttMessageStateWaitPubRel:
                if (MqttMessageShouldResend(client, msg))
                {
                    MqttClientSendPubRec(client, msg);
                }
                break;

            default:
                break;
        }
    }
}

static void MqttClientProcessOutMessages(MqttClient *client)
{
    MqttMessage *msg, *next;
    MqttPacket *packet;
    int inflight = MqttClientInflightMessageCount(client);

    TAILQ_FOREACH_SAFE(msg, &client->outMessages, chain, next)
    {
        switch (msg->state)
        {
            case MqttMessageStateQueued:
            {
                if (inflight >= client->maxInflight)
                {
                    continue;
                }
                /* State change from MqttMessageStatePublish happens after
                   the packet has been sent (in MqttClientSendPacket). */
                msg->state = MqttMessageStatePublish;
                packet = PublishToPacket(msg);
                MqttClientQueuePacket(client, packet);
                ++inflight;
                break;
            }

            case MqttMessageStateWaitPubAck:
            case MqttMessageStateWaitPubRec:
            {
                if (MqttMessageShouldResend(client, msg))
                {
                    msg->state = MqttMessageStatePublish;
                    packet = PublishToPacket(msg);
                    MqttClientQueuePacket(client, packet);
                }
                break;
            }

            case MqttMessageStateWaitPubComp:
            {
                if (MqttMessageShouldResend(client, msg))
                {
                    MqttClientSendPubRel(client, msg);
                }
                break;
            }
        }
    }
}

static void MqttClientProcessMessageQueue(MqttClient *client)
{
    MqttClientProcessInMessages(client);
    MqttClientProcessOutMessages(client);
}

static void MqttClientClearQueues(MqttClient *client)
{
    while (!SIMPLEQ_EMPTY(&client->sendQueue))
    {
        MqttPacket *packet = SIMPLEQ_FIRST(&client->sendQueue);
        SIMPLEQ_REMOVE_HEAD(&client->sendQueue, sendQueue);
        MqttPacketFree(packet);
    }

    while (!TAILQ_EMPTY(&client->outMessages))
    {
        MqttMessage *msg = TAILQ_FIRST(&client->outMessages);
        TAILQ_REMOVE(&client->outMessages, msg, chain);
        MqttMessageFree(msg);
    }

    while (!TAILQ_EMPTY(&client->inMessages))
    {
        MqttMessage *msg = TAILQ_FIRST(&client->inMessages);
        TAILQ_REMOVE(&client->inMessages, msg, chain);
        MqttMessageFree(msg);
    }
}

void MqttClientPause(MqttClient *client)
{
    assert(client != NULL);
    client->paused = 1;
}

void MqttClientResume(MqttClient *client)
{
    assert(client != NULL);
    client->paused = 0;
}
