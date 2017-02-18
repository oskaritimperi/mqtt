#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST basic_test()
{
    TestClient *client;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientSubscribe(client, topics[0], 2));
    ASSERT(TestClientPublish(client, 0, 0, topics[0], "msg0"));
    ASSERT(TestClientPublish(client, 1, 0, topics[0], "msg1"));
    ASSERT(TestClientPublish(client, 2, 0, topics[0], "msg2"));
    ASSERT(TestClientWait(client, 2000));
    ASSERT_EQ(3, TestClientMessageCount(client));
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(basic_test);
    GREATEST_MAIN_END();
}
