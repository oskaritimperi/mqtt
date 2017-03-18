#include "greatest.h"
#include "testclient.h"
#include "cleanup.c"
#include "topics.c"
#include <bstrlib/bstrlib.h>
#include "bstraux.h"

static const struct tagbstring message = bsStatic(
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas eu elit vel nisl fringilla ornare. Vestibulum eget sem lobortis, molestie velit in, gravida turpis. Donec ac sapien eu neque pellentesque dictum. Maecenas sed malesuada augue, nec ullamcorper libero. Donec consectetur sit amet orci non viverra. Morbi pharetra, urna ac luctus consequat, nibh urna semper metus, nec consectetur eros sapien in lorem. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce elit magna, fringilla vel velit ac, finibus interdum nibh. Donec sit amet volutpat elit. Sed sodales finibus nisl, ut vulputate tortor. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vel egestas tellus. Aliquam eget orci eget tortor porttitor ullamcorper in vel nulla. Cras facilisis tristique turpis vel molestie. Quisque suscipit orci orci, et convallis est eleifend sit amet."
"Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. In hac habitasse platea dictumst. Donec auctor ante odio, vitae tristique nisi egestas a. Suspendisse sit amet fermentum libero, viverra tempor neque. Nunc eleifend quam ac lacus ullamcorper fermentum. Integer lorem turpis, lobortis eget risus nec, auctor convallis sapien. In laoreet mauris at mi vehicula bibendum. Lorem ipsum dolor sit amet, consectetur adipiscing elit."
"Quisque commodo nisi vel tellus sodales, nec laoreet arcu gravida. Mauris vitae ligula nisl. Maecenas in euismod odio, vel vulputate arcu. Mauris vehicula tortor nec tempus euismod. Maecenas at tortor in libero pretium consequat a sed augue. Phasellus tortor erat, hendrerit id placerat id, pulvinar eget lacus. Curabitur rhoncus lobortis augue, hendrerit sodales tellus faucibus at. Donec a eros tellus. Sed at urna a lectus scelerisque lobortis."
"Duis accumsan ut augue sit amet suscipit. Cras tincidunt quam elementum magna faucibus eleifend. Etiam magna elit, commodo a tortor tempus, tempor vestibulum lorem. Nullam volutpat, libero a semper porttitor, neque turpis auctor augue, ut consequat diam nunc non tellus. Morbi nec varius ipsum, at imperdiet nisl. Fusce a est leo. Sed vitae turpis ligula. Vivamus eget eros id magna tincidunt consequat ut vel lorem. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Nam consectetur in tellus sit amet blandit. Cras cursus dictum ex, at iaculis sem ultricies quis. Fusce vitae pretium tellus, non cursus sem. Suspendisse ac dui eu quam semper eleifend ac sit amet orci. Nunc a nibh felis. Vivamus porta fermentum diam, vel commodo sem tincidunt ac."
"Nam dapibus, tellus nec pharetra efficitur, velit mauris faucibus nulla, ac sodales enim ex interdum tortor. Sed eget metus quis dolor euismod elementum vitae non felis. Nullam gravida diam sit amet suscipit iaculis. Quisque vehicula maximus lorem non volutpat. Vestibulum nec dui eu neque sodales finibus. Pellentesque eleifend fermentum erat, a tincidunt nisl luctus ultricies. Aliquam malesuada enim metus, nec pharetra orci dictum id. Lorem ipsum dolor sit amet, consectetur adipiscing elit. In neque urna, vehicula nec ante vel, porta dignissim lorem. Duis fringilla arcu nec tellus lacinia facilisis."
);

TEST big_message_test()
{
    TestClient *client;
    bstring encodedMessage;
    bstring fullMessage;
    int need = 1024 * 1024 * 3.5;

    fullMessage = bstrcpy(&message);
    bpattern(fullMessage, need);

    encodedMessage = bBase64Encode(fullMessage);

    printf("ENCODED MESSAGE SIZE %d\n", blength(encodedMessage));

    client = TestClientNew("clienta");
    ASSERT(TestClientConnect(client, "localhost", 1883, 60, 1));
    ASSERT(TestClientSubscribe(client, topics[0], 1));
    ASSERT(TestClientPublish(client, 1, 0, topics[0], bdata(encodedMessage)));
    ASSERT(TestClientWait(client, 2000));
    TestClientDisconnect(client);

    ASSERT_EQ(1, TestClientMessageCount(client));
    ASSERT_EQ(blength(encodedMessage), SIMPLEQ_FIRST(&client->messages)->size);
    ASSERT_MEM_EQ(bdata(encodedMessage),
                  SIMPLEQ_FIRST(&client->messages)->data,
                  blength(encodedMessage));

    TestClientFree(client);

    bdestroy(encodedMessage);
    bdestroy(fullMessage);

    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    cleanup();
    RUN_TEST(big_message_test);
    GREATEST_MAIN_END();
}
