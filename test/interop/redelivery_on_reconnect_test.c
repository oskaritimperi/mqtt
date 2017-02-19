#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST redelivery_on_reconnect_test()
{
    TestClient *client;

    client = TestClientNew("clientb");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 0));
    ASSERT(TestClientSubscribe(client, wildtopics[6], 2));
    MqttClientPause(client->client);
    ASSERT(TestClientPublish(client, 1, 0, topics[1], ""));
    ASSERT(TestClientPublish(client, 2, 0, topics[3], ""));
    ASSERT(TestClientWait(client, 1000));
    TestClientDisconnect(client);
    ASSERT_EQ(0, TestClientMessageCount(client));
    MqttClientResume(client->client);
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 0));
    ASSERT(TestClientWait(client, 3000));
    ASSERT_EQ(2, TestClientMessageCount(client));
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(redelivery_on_reconnect_test);
    GREATEST_MAIN_END();
}
