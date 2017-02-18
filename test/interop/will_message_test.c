#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST will_message_test()
{
    TestClient *clienta, *clientb;

    clienta = TestClientNew("clienta");
    ASSERT_EQ(0, MqttClientSetWill(clienta->client, topics[2],
                                   "client disconnected", 19, 0, 0));
    ASSERT(TestClientConnect(clienta, "localhost", 1883, 2, 1));

    clientb = TestClientNew("clientb");
    ASSERT(TestClientConnect(clientb, "localhost", 1883, 60, 0));
    ASSERT(TestClientSubscribe(clientb, topics[2], 2));

    ASSERT(TestClientWait(clientb, 500));

    TestClientFree(clienta);

    ASSERT(TestClientWait(clientb, 5000));

    TestClientDisconnect(clientb);

    ASSERT_EQ(1, TestClientMessageCount(clientb));

    TestClientFree(clientb);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(will_message_test);
    GREATEST_MAIN_END();
}
