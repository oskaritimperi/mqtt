#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST unsubscribe_test()
{
    TestClient *client;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientSubscribe(client, topics[0], 2));
    ASSERT(TestClientPublish(client, 2, 0, topics[0], "msg"));
    ASSERT(TestClientUnsubscribe(client, topics[0]));
    ASSERT(TestClientPublish(client, 2, 0, topics[0], "msg"));
    TestClientDisconnect(client);
    ASSERT_EQ(1, TestClientMessageCount(client));
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(unsubscribe_test);
    GREATEST_MAIN_END();
}
