#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"

TEST zero_length_clientid_test()
{
    TestClient *client;

    client = TestClientNew("");
    ASSERT_FALSE(TestClientConnect(client, "localhost", 1883, 60, 0));
    TestClientFree(client);

    client = TestClientNew("");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(zero_length_clientid_test);
    GREATEST_MAIN_END();
}
