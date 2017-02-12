#include "mqtt.h"
#include "packet.h"
#include "stream.h"
#include "socketstream.h"
#include "socket.h"
#include "misc.h"
#include "serialize.h"
#include "deserialize.h"
#include "log.h"

#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>

#if (LOG_LEVEL == LOG_LEVEL_DEBUG) && !defined(PRId64)
#error define PRId64 for your platform
#endif

#ifdef __APPLE__
#include <sys/select.h>
#endif

TAILQ_HEAD(MessageList, MqttPacket);
typedef struct MessageList MessageList;

struct MqttClient
{
    SocketStream stream;
    /* client id, NULL if we want to have server generated id */
    char *clientId;
    /* set to 1 if we want to have a clean session */
    int cleanSession;
    /* remote host and port */
    char *host;
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
    MessageList outMessages;
    /* received messages that are not done yet  */
    MessageList inMessages;
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
};

enum MessageState
{
    MessageStateQueued = 100,
    MessageStateSend,
    MessageStateSent
};

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet);
static int MqttClientQueueSimplePacket(MqttClient *client, int type);
static int MqttClientSendPacket(MqttClient *client, MqttPacket *packet);
static int MqttClientRecvPacket(MqttClient *client);
static uint16_t MqttClientNextPacketId(MqttClient *client);
static void MqttClientProcessMessageQueue(MqttClient *client);

static inline int MqttClientInflightMessageCount(MqttClient *client)
{
    MqttPacket *packet;
    int queued = 0;
    int inMessagesCount = 0;
    int outMessagesCount = 0;

    TAILQ_FOREACH(packet, &client->outMessages, messages)
    {
        if (packet->state == MessageStateQueued)
        {
            ++queued;
        }

        ++outMessagesCount;
    }

    TAILQ_FOREACH(packet, &client->inMessages, messages)
    {
        ++inMessagesCount;
    }

    return inMessagesCount + outMessagesCount - queued;
}

static char *CopyString(const char *s, int n)
{
    char *result = NULL;

    if (n < 0)
        n = strlen(s);

    result = malloc(n+1);

    assert(result != NULL);

    memcpy(result, s, n);
    result[n] = '\0';

    return result;
}

MqttClient *MqttClientNew(const char *clientId, int cleanSession)
{
    MqttClient *client;

    client = calloc(1, sizeof(*client));

    if (!client)
    {
        return NULL;
    }

    if (clientId == NULL)
    {
        client->clientId = CopyString("", 0);
    }
    else
    {
        client->clientId = CopyString(clientId, -1);
    }

    client->cleanSession = cleanSession;

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
    if (client->clientId)
    {
        free(client->clientId);
    }

    if (client->host)
    {
        free(client->host);
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

int MqttClientConnect(MqttClient *client, const char *host, short port,
                      int keepAlive)
{
    int sock;
    MqttPacketConnect *packet;

    assert(client != NULL);
    assert(host != NULL);

    client->host = CopyString(host, -1);
    client->port = port;
    client->keepAlive = keepAlive;

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

    packet = (MqttPacketConnect *) MqttPacketNew(MqttPacketTypeConnect);

    if (!packet)
        return -1;

    if (client->cleanSession)
    {
        packet->connectFlags |= 0x02;
    }

    packet->keepAlive = client->keepAlive;

    if (StringBufInitFromCString(&packet->clientId, client->clientId, -1) == -1)
    {
        free(packet);
        return -1;
    }

    MqttClientQueuePacket(client, &packet->base);

    return 0;
}

int MqttClientDisconnect(MqttClient *client)
{
    LOG_DEBUG("disconnecting");
    return MqttClientQueueSimplePacket(client, MqttPacketTypeDisconnect);
}

int MqttClientRunOnce(MqttClient *client)
{
    fd_set rfd, wfd;
    struct timeval tv;
    int rv;

    assert(client != NULL);

    if (client->stream.sock == -1)
    {
        LOG_ERROR("invalid socket");
        return -1;
    }

    FD_ZERO(&rfd);
    FD_ZERO(&wfd);

    FD_SET(client->stream.sock, &rfd);

    /* Handle outMessages and inMessages, moving queued messages to sendQueue
       if there are less than maxInflight number of messages in flight */
    MqttClientProcessMessageQueue(client);

    if (SIMPLEQ_EMPTY(&client->sendQueue))
    {
        LOG_DEBUG("nothing to write");
    }
    else
    {
        FD_SET(client->stream.sock, &wfd);
    }

    // TODO: break select when queuing packets (need to protect queue with mutex
    // to allow queuing packets from another thread)

    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = client->keepAlive;
    tv.tv_usec = 0;

    LOG_DEBUG("selecting");
    rv = select(client->stream.sock+1, &rfd, &wfd, NULL, &tv);

    if (rv == -1)
    {
        LOG_ERROR("select failed");
        return -1;
    }
    else if (rv)
    {
        LOG_DEBUG("select rv=%d", rv);

        if (FD_ISSET(client->stream.sock, &wfd))
        {
            MqttPacket *packet;

            LOG_DEBUG("socket writable");

            packet = SIMPLEQ_FIRST(&client->sendQueue);

            if (packet)
            {
                SIMPLEQ_REMOVE_HEAD(&client->sendQueue, sendQueue);

                if (MqttClientSendPacket(client, packet) == -1)
                {
                    LOG_ERROR("MqttClientSendPacket failed");
                    client->stopped = 1;
                }
            }
        }

        if (FD_ISSET(client->stream.sock, &rfd))
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
            int64_t elapsed = GetCurrentTime() - client->lastPacketSentTime;
            if (elapsed/1000 >= client->keepAlive)
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
        if (MqttClientRunOnce(client) == -1)
            return -1;
    }

    return 0;
}

int MqttClientSubscribe(MqttClient *client, const char *topicFilter,
                        int qos)
{
    MqttPacketSubscribe *packet = NULL;

    assert(client != NULL);
    assert(topicFilter != NULL);
    assert(qos >= 0 && qos <= 2);

    packet = (MqttPacketSubscribe *) MqttPacketWithIdNew(
        MqttPacketTypeSubscribe, MqttClientNextPacketId(client));

    if (!packet)
        return -1;

    if (StringBufInitFromCString(&packet->topicFilter, topicFilter, -1) == -1)
    {
        MqttPacketFree((MqttPacket *) packet);
        return -1;
    }

    packet->qos = qos;

    MqttClientQueuePacket(client, (MqttPacket *) packet);

    TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet, messages);

    return MqttPacketId(packet);
}

int MqttClientUnsubscribe(MqttClient *client, const char *topicFilter)
{
    MqttPacketUnsubscribe *packet = NULL;

    assert(client != NULL);
    assert(topicFilter != NULL);

    packet = (MqttPacketUnsubscribe *) MqttPacketWithIdNew(
        MqttPacketTypeUnsubscribe, MqttClientNextPacketId(client));

    if (StringBufInitFromCString(&packet->topicFilter, topicFilter, -1) == -1)
    {
        MqttPacketFree((MqttPacket *) packet);
        return -1;
    }

    MqttClientQueuePacket(client, (MqttPacket *) packet);

    TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet, messages);

    return MqttPacketId(packet);
}

static inline int MqttClientOutMessagesLen(MqttClient *client)
{
    MqttPacket *packet;
    int count = 0;
    TAILQ_FOREACH(packet, &client->outMessages, messages)
    {
        ++count;
    }
    return count;
}

int MqttClientPublish(MqttClient *client, int qos, int retain,
                      const char *topic, const void *data, size_t size)
{
    MqttPacketPublish *packet;

    assert(client != NULL);

    /* first check if the queue is already full */
    if (qos > 0 && client->maxQueued > 0 &&
        MqttClientOutMessagesLen(client) >= client->maxQueued)
    {
        return -1;
    }

    if (qos > 0)
    {
        packet = (MqttPacketPublish *) MqttPacketWithIdNew(
            MqttPacketTypePublish, MqttClientNextPacketId(client));
    }
    else
    {
        packet = (MqttPacketPublish *) MqttPacketNew(MqttPacketTypePublish);
    }

    if (!packet)
        return -1;

    packet->qos = qos;
    packet->retain = retain;

    if (StringBufInitFromCString(&packet->topicName, topic, -1) == -1)
    {
        MqttPacketFree((MqttPacket *) packet);
        return -1;
    }

    if (StringBufInitFromData(&packet->message, data, size) == -1)
    {
        MqttPacketFree((MqttPacket *) packet);
        return -1;
    }

    if (qos > 0)
    {
        /* check how many messages there are coming in and going out currently
           that are not yet done */
        if (client->maxInflight == 0 ||
            MqttClientInflightMessageCount(client) < client->maxInflight)
        {
            LOG_DEBUG("setting message (%d) state to MessageStateSend",
                      MqttPacketId(packet));
            packet->base.state = MessageStateSend;
        }
        else
        {
            LOG_DEBUG("setting message (%d) state to MessageStateQueued",
                      MqttPacketId(packet));
            packet->base.state = MessageStateQueued;
        }

        /* add the message to the outMessages queue to wait for processing */
        TAILQ_INSERT_TAIL(&client->outMessages, (MqttPacket *) packet,
            messages);
    }
    else
    {
        MqttClientQueuePacket(client, (MqttPacket *) packet);
    }

    if (qos > 0)
        return MqttPacketId(packet);

    return 0;
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

static void MqttClientQueuePacket(MqttClient *client, MqttPacket *packet)
{
    assert(client != NULL);
    LOG_DEBUG("queuing packet %s", MqttPacketName(packet->type));
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

static int MqttClientSendPacket(MqttClient *client, MqttPacket *packet)
{
    if (MqttPacketSerialize(packet, &client->stream.base) == -1)
        return -1;

    packet->sentAt = GetCurrentTime();
    client->lastPacketSentTime = packet->sentAt;

    if (packet->type == MqttPacketTypeDisconnect)
    {
        client->stopped = 1;
    }

    /* If the packet is not on any message list, it can be removed after
       sending. */
    if (TAILQ_NEXT(packet, messages) == NULL &&
        TAILQ_PREV(packet, MessageList, messages) == NULL &&
        TAILQ_FIRST(&client->inMessages) != packet &&
        TAILQ_FIRST(&client->outMessages) != packet)
    {
        LOG_DEBUG("freeing packet %s after sending",
            MqttPacketName(MqttPacketType(packet)));
        MqttPacketFree(packet);
    }

    return 0;
}

static void MqttClientHandleConnAck(MqttClient *client,
    MqttPacketConnAck *packet)
{
    client->sessionPresent = packet->connAckFlags & 1;

    LOG_DEBUG("sessionPresent:%d", client->sessionPresent);

    if (client->onConnect)
    {
        LOG_DEBUG("calling onConnect rc:%d", packet->returnCode);
        client->onConnect(client, packet->returnCode, client->sessionPresent);
    }
}

static void MqttClientHandlePingResp(MqttClient *client)
{
    LOG_DEBUG("got ping response");
    client->pingSent = 0;
}

static void MqttClientHandleSubAck(MqttClient *client, MqttPacketSubAck *packet)
{
    MqttPacket *sub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(sub, &client->outMessages, messages)
    {
        if (MqttPacketType(sub) == MqttPacketTypeSubscribe &&
            MqttPacketId(sub) == MqttPacketId(packet))
        {
            break;
        }
    }

    if (!sub)
    {
        LOG_ERROR("SUBSCRIBE with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, sub, messages);
        MqttPacketFree(sub);

        if (client->onSubscribe)
        {
            LOG_DEBUG("calling onSubscribe id:%d rc:%d", MqttPacketId(packet),
                packet->returnCode);
            client->onSubscribe(client, MqttPacketId(packet),
                                packet->returnCode);
        }
    }

    MqttPacketFree((MqttPacket *) packet);
}

static void MqttClientHandlePublish(MqttClient *client, MqttPacketPublish *packet)
{
    if (MqttPacketPublishQos(packet) == 2)
    {
        /* Check if we have sent a PUBREC previously with the same id. If we
           have, we have to resend the PUBREC. We must not call the onMessage
           callback again. */

        MqttPacket *pubRec;

        TAILQ_FOREACH(pubRec, &client->inMessages, messages)
        {
            if (MqttPacketId(pubRec) == MqttPacketId(packet) &&
                MqttPacketType(pubRec) == MqttPacketTypePubRec)
            {
                break;
            }
        }

        if (pubRec)
        {
            LOG_DEBUG("resending PUBREC id:%d", MqttPacketId(packet));
            // MqttPacketWithId *pubRec = (MqttPacketWithId *) pubRecNode->packet;
            MqttClientQueuePacket(client, pubRec);
            MqttPacketFree((MqttPacket *) packet);
            return;
        }
    }

    if (client->onMessage)
    {
        LOG_DEBUG("calling onMessage");
        client->onMessage(client,
            packet->topicName.data,
            packet->message.data,
            packet->message.len);
    }

    if (MqttPacketPublishQos(packet) > 0)
    {
        int type = (MqttPacketPublishQos(packet) == 1) ? MqttPacketTypePubAck :
            MqttPacketTypePubRec;

        MqttPacket *resp = MqttPacketWithIdNew(type, MqttPacketId(packet));

        if (MqttPacketPublishQos(packet) == 2)
        {
            /* append to inMessages as we need a reply to this response */
            TAILQ_INSERT_TAIL(&client->inMessages, resp, messages);
        }

        MqttClientQueuePacket(client, resp);
    }

    MqttPacketFree((MqttPacket *) packet);
}

static void MqttClientHandlePubAck(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pub;

    TAILQ_FOREACH(pub, &client->outMessages, messages)
    {
        if (MqttPacketId(pub) == MqttPacketId(packet) &&
            MqttPacketType(pub) == MqttPacketTypePublish)
        {
            break;
        }
    }

    if (!pub)
    {
        LOG_ERROR("PUBLISH with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, pub, messages);
        MqttPacketFree(pub);

        if (client->onPublish)
        {
            client->onPublish(client, MqttPacketId(packet));
        }
    }

    MqttPacketFree(packet);
}

static void MqttClientHandlePubRec(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(pub, &client->outMessages, messages)
    {
        if (MqttPacketId(pub) == MqttPacketId(packet) &&
            MqttPacketType(pub) == MqttPacketTypePublish)
        {
            break;
        }
    }

    if (!pub)
    {
        LOG_ERROR("PUBLISH with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        MqttPacket *pubRel;

        TAILQ_REMOVE(&client->outMessages, pub, messages);
        MqttPacketFree(pub);

        pubRel = MqttPacketWithIdNew(MqttPacketTypePubRel, MqttPacketId(packet));
        pubRel->state = MessageStateSend;

        TAILQ_INSERT_TAIL(&client->outMessages, pubRel, messages);
    }

    MqttPacketFree(packet);
}

static void MqttClientHandlePubRel(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pubRec;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(pubRec, &client->inMessages, messages)
    {
        if (MqttPacketId(pubRec) == MqttPacketId(packet) &&
            MqttPacketType(pubRec) == MqttPacketTypePublish)
        {
            break;
        }
    }

    if (!pubRec)
    {
        MqttPacket *pubComp;

        TAILQ_FOREACH(pubComp, &client->inMessages, messages)
        {
            if (MqttPacketId(pubComp) == MqttPacketId(packet) &&
                MqttPacketType(pubComp) == MqttPacketTypePubComp)
            {
                break;
            }
        }

        if (pubComp)
        {
            MqttClientQueuePacket(client, pubComp);
        }
        else
        {
            LOG_ERROR("PUBREC with id:%d not found", MqttPacketId(packet));
            client->stopped = 1;
        }
    }
    else
    {
        MqttPacket *pubComp;

        TAILQ_REMOVE(&client->inMessages, pubRec, messages);
        MqttPacketFree(pubRec);

        pubComp = MqttPacketWithIdNew(MqttPacketTypePubComp,
                                      MqttPacketId(packet));

        TAILQ_INSERT_TAIL(&client->inMessages, pubComp, messages);

        MqttClientQueuePacket(client, pubComp);
    }

    MqttPacketFree(packet);
}

static void MqttClientHandlePubComp(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *pubRel;

    TAILQ_FOREACH(pubRel, &client->outMessages, messages)
    {
        if (MqttPacketId(pubRel) == MqttPacketId(packet) &&
            MqttPacketType(pubRel) == MqttPacketTypePubRel)
        {
            break;
        }
    }

    if (!pubRel)
    {
        LOG_ERROR("PUBREL with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, pubRel, messages);
        MqttPacketFree(pubRel);

        if (client->onPublish)
        {
            LOG_DEBUG("calling onPublish id:%d", MqttPacketId(packet));
            client->onPublish(client, MqttPacketId(packet));
        }
    }

    MqttPacketFree(packet);
}

static void MqttClientHandleUnsubAck(MqttClient *client, MqttPacket *packet)
{
    MqttPacket *sub;

    assert(client != NULL);
    assert(packet != NULL);

    TAILQ_FOREACH(sub, &client->outMessages, messages)
    {
        if (MqttPacketId(sub) == MqttPacketId(packet) &&
            MqttPacketType(sub) == MqttPacketTypeUnsubscribe)
        {
            break;
        }
    }

    if (!sub)
    {
        LOG_ERROR("UNSUBSCRIBE with id:%d not found", MqttPacketId(packet));
        client->stopped = 1;
    }
    else
    {
        TAILQ_REMOVE(&client->outMessages, sub, messages);
        MqttPacketFree(sub);

        if (client->onUnsubscribe)
        {
            LOG_DEBUG("calling onUnsubscribe id:%d", MqttPacketId(packet));
            client->onUnsubscribe(client, MqttPacketId(packet));
        }
    }

    MqttPacketFree(packet);
}

static int MqttClientRecvPacket(MqttClient *client)
{
    MqttPacket *packet = NULL;

    if (MqttPacketDeserialize(&packet, (Stream *) &client->stream) == -1)
        return -1;

    LOG_DEBUG("received packet %s", MqttPacketName(packet->type));

    switch (MqttPacketType(packet))
    {
        case MqttPacketTypeConnAck:
            MqttClientHandleConnAck(client, (MqttPacketConnAck *) packet);
            break;

        case MqttPacketTypePingResp:
            MqttClientHandlePingResp(client);
            break;

        case MqttPacketTypeSubAck:
            MqttClientHandleSubAck(client, (MqttPacketSubAck *) packet);
            break;

        case MqttPacketTypePublish:
            MqttClientHandlePublish(client, (MqttPacketPublish *) packet);
            break;

        case MqttPacketTypePubAck:
            MqttClientHandlePubAck(client, packet);
            break;

        case MqttPacketTypePubRec:
            MqttClientHandlePubRec(client, packet);
            break;

        case MqttPacketTypePubRel:
            MqttClientHandlePubRel(client, packet);
            break;

        case MqttPacketTypePubComp:
            MqttClientHandlePubComp(client, packet);
            break;

        case MqttPacketTypeUnsubAck:
            MqttClientHandleUnsubAck(client, packet);
            break;

        default:
            LOG_DEBUG("unhandled packet type=%d", MqttPacketType(packet));
            break;
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

static int64_t MqttPacketTimeSinceSent(MqttPacket *packet)
{
    int64_t now = GetCurrentTime();
    return now - packet->sentAt;
}

static void MqttClientProcessInMessages(MqttClient *client)
{
    MqttPacket *packet, *next;

    LOG_DEBUG("processing inMessages");

    TAILQ_FOREACH_SAFE(packet, &client->inMessages, messages, next)
    {
        LOG_DEBUG("packet type:%s id:%d",
            MqttPacketName(MqttPacketType(packet)),
            MqttPacketId(packet));

        if (MqttPacketType(packet) == MqttPacketTypePubComp)
        {
            int64_t elapsed = MqttPacketTimeSinceSent(packet);
            if (packet->sentAt > 0 &&
                elapsed >= client->retryTimeout*1000)
            {
                LOG_DEBUG("freeing PUBCOMP with id:%d elapsed:%" PRId64,
                    MqttPacketId(packet), elapsed);

                TAILQ_REMOVE(&client->inMessages, packet, messages);

                MqttPacketFree(packet);
            }
        }
    }
}

static int MqttPacketShouldResend(MqttClient *client, MqttPacket *packet)
{
    if (packet->sentAt > 0 &&
        MqttPacketTimeSinceSent(packet) > client->retryTimeout*1000)
    {
        return 1;
    }

    return 0;
}

static void MqttClientProcessOutMessages(MqttClient *client)
{
    MqttPacket *packet, *next;
    int inflight = MqttClientInflightMessageCount(client);

    LOG_DEBUG("processing outMessages inflight:%d", inflight);

    TAILQ_FOREACH_SAFE(packet, &client->outMessages, messages, next)
    {
        LOG_DEBUG("packet type:%s id:%d state:%d",
            MqttPacketName(MqttPacketType(packet)),
            MqttPacketId(packet),
            packet->state);

        switch (packet->state)
        {
            case MessageStateQueued:
                if (inflight >= client->maxInflight)
                {
                    LOG_DEBUG("cannot dequeue %s/%d",
                        MqttPacketName(MqttPacketType(packet)),
                        MqttPacketId(packet));
                    break;
                }
                else
                {
                    /* If there's less than maxInflight messages currently
                       inflight, we can dequeue some messages by falling
                       through to MessageStateSend. */
                    LOG_DEBUG("dequeuing %s (%d)",
                        MqttPacketName(MqttPacketType(packet)),
                        MqttPacketId(packet));
                    ++inflight;
                }

            case MessageStateSend:
                packet->state = MessageStateSent;
                MqttClientQueuePacket(client, packet);
                break;

            case MessageStateSent:
                if (MqttPacketShouldResend(client, packet))
                {
                    packet->state = MessageStateSend;
                }
                break;

            default:
                break;
        }
    }
}

static void MqttClientProcessMessageQueue(MqttClient *client)
{
    MqttClientProcessInMessages(client);
    MqttClientProcessOutMessages(client);
}
