#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"

TEST username_and_password_test()
{
    TestClient *client;

    client = TestClientNew("clienta");
    ASSERT_EQ(0, MqttClientSetAuth(client->client, "myusername", NULL));
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    TestClientDisconnect(client);
    ASSERT_EQ(0, MqttClientSetAuth(client->client, "myusername", "mypassword"));
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
    RUN_TEST(username_and_password_test);
    GREATEST_MAIN_END();
}
