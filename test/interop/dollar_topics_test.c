#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"
#include <bstrlib/bstrlib.h>

TEST dollar_topics_test()
{
    TestClient *client;
    bstring topic;

    topic = bfromcstr("$");
    bcatcstr(topic, topics[1]);

    client = TestClientNew("clientb");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 0));
    ASSERT(TestClientSubscribe(client, wildtopics[5], 2));
    ASSERT(TestClientWait(client, 1000));
    ASSERT(TestClientPublish(client, 1, 0, bdata(topic), ""));
    ASSERT(TestClientWait(client, 2000));
    TestClientDisconnect(client);
    ASSERT_EQ(0, TestClientMessageCount(client));
    TestClientFree(client);

    bdestroy(topic);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(dollar_topics_test);
    GREATEST_MAIN_END();
}
