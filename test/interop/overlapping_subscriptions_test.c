#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST overlapping_subscriptions_test()
{
    TestClient *client;
    int count;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientSubscribe(client, wildtopics[6], 2));
    ASSERT(TestClientSubscribe(client, wildtopics[0], 1));
    ASSERT(TestClientPublish(client, 2, 0, topics[3],
                             "overlapping topic filters"));
    ASSERT(TestClientWait(client, 1000));
    TestClientDisconnect(client);
    count = TestClientMessageCount(client);
    ASSERT(count == 1 || count == 2);
    if (count == 1)
    {
        printf("this server sends one message for for all matching "
               "overlapping subscriptions, not one for each\n");
        ASSERT_EQ(2, SIMPLEQ_FIRST(&client->messages)->qos);
    }
    else
    {
        printf("this server sends one message per each matching overlapping "
               "subscription\n");
        int qos1 = SIMPLEQ_FIRST(&client->messages)->qos;
        int qos2 = SIMPLEQ_NEXT(SIMPLEQ_FIRST(&client->messages), chain)->qos;
        ASSERT((qos1 == 1 && qos2 == 2) || (qos1 == 2 && qos2 == 1));
    }
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(overlapping_subscriptions_test);
    GREATEST_MAIN_END();
}
