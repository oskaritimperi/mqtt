#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST ping_test()
{
    TestClient *client;

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 1, 1));
    ASSERT(TestClientWait(client, 5000));
    TestClientDisconnect(client);
    TestClientFree(client);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(ping_test);
    GREATEST_MAIN_END();
}
