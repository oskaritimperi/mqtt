#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST keepalive_test()
{
    TestClient *clienta, *clientb;

    clienta = TestClientNew("clienta");
    ASSERT_EQ(0, MqttClientSetWill(clienta->client, topics[4],
                                   "client disconnected", 19, 0, 0));
    ASSERT(TestClientConnect(clienta, "localhost", 1883, 2, 1));

    clientb = TestClientNew("clientb");
    ASSERT(TestClientConnect(clientb, "localhost", 1883, 0, 1));
    ASSERT(TestClientSubscribe(clientb, topics[4], 2));

    ASSERT(TestClientWait(clientb, 15000));

    TestClientDisconnect(clientb);

    ASSERT_EQ(1, TestClientMessageCount(clientb));

    TestClientFree(clienta);
    TestClientFree(clientb);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(keepalive_test);
    GREATEST_MAIN_END();
}
