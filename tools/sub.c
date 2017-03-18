#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "optparse.h"

struct options
{
    int qos;
    const char *topic;
    int clean;
    const char *client_id;
};

void onConnect(MqttClient *client, MqttConnectionStatus status,
               int sessionPresent)
{
    struct options *options = (struct options *) MqttClientGetUserData(client);
    (void) client;
    printf("onConnect rv=%d sessionPresent=%d\n", status, sessionPresent);
    MqttClientSubscribe(client, options->topic, options->qos);
}

void onSubscribe(MqttClient *client, int id, int *qos, int count)
{
    (void) client;
    printf("onSubscribe id=%d\n", id);
}

void onMessage(MqttClient *client, const char *topic, const void *data,
    size_t size, int qos, int retain)
{
    (void) client;
    printf("onMessage topic=<%s> message=<%.*s>\n", topic, (int) size,
        (char *) data);
    // MqttClientUnsubscribe(client, topic);
}

void usage(const char *prog)
{
    fprintf(stderr, "%s [--qos QOS] [--topic TOPIC] [--clean] [--id ID]\n",
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
        { "no-clean", 'n', OPTPARSE_NONE },
        { "id", 'i', OPTPARSE_REQUIRED },
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

            case 'n':
                options->clean = 0;
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
    options.topic = "$SYS/broker/load/messages/#";
    options.clean = 1;
    options.client_id = NULL;

    parse_args(&options, argc, argv);

    client = MqttClientNew(options.client_id);

    MqttClientSetOnConnect(client, onConnect);
    MqttClientSetOnSubscribe(client, onSubscribe);
    MqttClientSetOnMessage(client, onMessage);
    MqttClientSetUserData(client, &options);

    MqttClientConnect(client, "test.mosquitto.org", 1883, 60, options.clean);

    MqttClientRun(client);

    MqttClientFree(client);

    return 0;
}
