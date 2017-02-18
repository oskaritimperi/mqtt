#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST retained_message_test()
{
    TestClient *client;

    client = TestClientNew("retained_message_test");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientPublish(client, 0, 1, topics[1], "qos0"));
    ASSERT(TestClientPublish(client, 1, 1, topics[2], "qos1"));
    ASSERT(TestClientPublish(client, 2, 1, topics[3], "qos2"));
    ASSERT(TestClientWait(client, 1000));
    ASSERT(TestClientSubscribe(client, wildtopics[5], 2));
    ASSERT(TestClientWait(client, 1000));
    ASSERT_EQ(3, TestClientMessageCount(client));
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("retained_message_test");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientPublish(client, 0, 1, topics[1], ""));
    ASSERT(TestClientPublish(client, 1, 1, topics[2], ""));
    ASSERT(TestClientPublish(client, 2, 1, topics[3], ""));
    ASSERT(TestClientWait(client, 1000));
    ASSERT(TestClientSubscribe(client, wildtopics[5], 2));
    ASSERT(TestClientWait(client, 1000));
    ASSERT_EQ(0, TestClientMessageCount(client));
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(retained_message_test);
    GREATEST_MAIN_END();
}
