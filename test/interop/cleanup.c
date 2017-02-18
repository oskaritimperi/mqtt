static void cleanup()
{
    TestClient *client;
    Message *msg;

    /* Clean client state */

    client = TestClientNew("clienta");
    TestClientConnect(client, "localhost", 1883, 60, 1);
    TestClientWait(client, 100);
    TestClientDisconnect(client);
    TestClientFree(client);

    client = TestClientNew("clientb");
    TestClientConnect(client, "localhost", 1883, 60, 1);
    TestClientWait(client, 100);
    TestClientDisconnect(client);
    TestClientFree(client);

    /* Clean retained messages */

    client = TestClientNew("clean-retained");
    TestClientConnect(client, "localhost", 1883, 60, 1);
    TestClientSubscribe(client, "#", 0);
    TestClientWait(client, 2000);

    SIMPLEQ_FOREACH(msg, &client->messages, chain)
    {
        if (msg->retain)
        {
            printf("deleting retained message for topic %s\n", msg->topic);
            TestClientPublish(client, 0, 1, msg->topic, "");
        }
    }

    TestClientDisconnect(client);

    TestClientFree(client);
}
