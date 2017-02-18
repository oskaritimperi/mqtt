#include "greatest.h"
#include "testclient.h"

TEST basic_test()
{
    TestClient *client;

    client = TestClientNew("basic_test");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("basic_test");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientSubscribe(client, "basic_test/basic_test", 2));
    ASSERT(TestClientPublish(client, 0, 0, "basic_test/basic_test", "msg0"));
    ASSERT(TestClientPublish(client, 1, 0, "basic_test/basic_test", "msg1"));
    ASSERT(TestClientPublish(client, 2, 0, "basic_test/basic_test", "msg2"));
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
    RUN_TEST(basic_test);
    GREATEST_MAIN_END();
}
