#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "optparse.h"

struct options
{
    int qos;
    int retain;
    const char *topic;
    const char *message;
    const char *client_id;
};

void onConnect(MqttClient *client, MqttConnectionStatus status,
               int sessionPresent)
{
    struct options *options = (struct options *) MqttClientGetUserData(client);
    (void) client;
    printf("onConnect rv=%d sessionPresent=%d\n", status, sessionPresent);
    MqttClientPublishCString(client, options->qos, options->retain,
        options->topic, options->message);
    if (options->qos == 0)
        MqttClientDisconnect(client);
}

void onPublish(MqttClient *client, int id)
{
    printf("onPublish id=%d\n", id);
    MqttClientDisconnect(client);
}

void usage(const char *prog)
{
    fprintf(stderr, "%s [--qos QOS] [--retain] [--topic TOPIC] [--message MESSAGE]\n",
        prog);
    exit(1);
}

static void parse_args(struct options *options, int argc, char **argv)
{
    int option;

    struct optparse_long longopts[] =
    {
        { "qos", 'q', OPTPARSE_REQUIRED },
        { "topic", 't', OPTPARSE_REQUIRED },
        { "message", 'm', OPTPARSE_REQUIRED },
        { "id", 'i', OPTPARSE_REQUIRED },
        { "retain", 'r', OPTPARSE_NONE },
        { "help", 'h', OPTPARSE_NONE },
        { NULL }
    };

    struct optparse parser;

    optparse_init(&parser, argv);

    while ((option = optparse_long(&parser, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'q':
                options->qos = strtol(parser.optarg, NULL, 10);
                if (options->qos < 0 || options->qos > 2)
                {
                    fprintf(stderr, "invalid qos: %s\n", parser.optarg);
                    exit(1);
                }
                break;

            case 't':
                options->topic = parser.optarg;
                break;

            case 'm':
                options->message = parser.optarg;
                break;

            case 'r':
                options->retain = 1;
                break;

            case 'i':
                options->client_id = parser.optarg;
                break;

            case 'h':
                usage(argv[0]);
                break;

            case '?':
                fprintf(stderr, "%s: %s\n", argv[0], parser.errmsg);
                usage(argv[0]);
                break;
        }
    }
}

int main(int argc, char **argv)
{
    MqttClient *client;
    struct options options;

    options.qos = 0;
    options.retain = 0;
    options.topic = "my/topic";
    options.message = "hello, world!";
    options.client_id = NULL;

    parse_args(&options, argc, argv);

    client = MqttClientNew(options.client_id);

    MqttClientSetOnConnect(client, onConnect);
    MqttClientSetOnPublish(client, onPublish);
    MqttClientSetUserData(client, &options);

    MqttClientConnect(client, "test.mosquitto.org", 1883, 60, 1);

    MqttClientRun(client);

    MqttClientFree(client);

    return 0;
}
