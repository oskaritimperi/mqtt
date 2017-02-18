#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST offline_message_queueing_test()
{
    TestClient *client;
    int count;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 0));
    ASSERT(TestClientSubscribe(client, wildtopics[5], 2));
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("clientb");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientPublish(client, 0, 0, topics[1], "qos0"));
    ASSERT(TestClientPublish(client, 1, 0, topics[2], "qos1"));
    ASSERT(TestClientPublish(client, 2, 0, topics[3], "qos2"));
    ASSERT(TestClientWait(client, 1000));
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 0));
    ASSERT(TestClientWait(client, 2000));
    TestClientDisconnect(client);
    count = TestClientMessageCount(client);
    TestClientFree(client);

    ASSERT(count == 2 || count == 3);

    printf("this server %s queueing QoS 0 messages for offline clients\n",
        count == 2 ? "is not" : "is");

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(offline_message_queueing_test);
    GREATEST_MAIN_END();
}
