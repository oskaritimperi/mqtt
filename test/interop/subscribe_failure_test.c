#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST subscribe_failure_test()
{
    TestClient *client;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT_FALSE(TestClientSubscribe(client, nosubscribe_topics[0], 2));
    ASSERT_EQ(MqttSubscriptionFailure, client->subStatus[0]);
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(subscribe_failure_test);
    GREATEST_MAIN_END();
}
