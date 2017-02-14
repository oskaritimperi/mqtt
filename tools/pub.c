#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "getopt.h"

struct options
{
    int qos;
    int retain;
    const char *topic;
    const char *message;
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

int main(int argc, char **argv)
{
    MqttClient *client;
    const char *opt;
    struct options options;

    options.qos = 0;
    options.retain = 0;
    options.topic = "my/topic";
    options.message = "hello, world!";

    while ((opt = GETOPT(argc, argv)) != NULL)
    {
        GETOPT_SWITCH(opt)
        {
            GETOPT_OPTARG("--qos"):
                options.qos = strtol(optarg, NULL, 10);
                if (options.qos < 0 || options.qos > 2)
                {
                    fprintf(stderr, "invalid qos: %s\n", optarg);
                    return 1;
                }
                break;

            GETOPT_OPT("--retain"):
                options.retain = 1;
                break;

            GETOPT_OPTARG("--topic"):
                options.topic = optarg;
                break;

            GETOPT_OPTARG("--message"):
                options.message = optarg;
                break;

            GETOPT_MISSING_ARG:
                fprintf(stderr, "missing argument to: %s\n", opt);

            GETOPT_DEFAULT:
                usage(argv[0]);
                break;
        }
    }

    client = MqttClientNew(NULL, 1);

    MqttClientSetOnConnect(client, onConnect);
    MqttClientSetOnPublish(client, onPublish);
    MqttClientSetUserData(client, &options);

    MqttClientConnect(client, "test.mosquitto.org", 1883, 60);

    MqttClientRun(client);

    MqttClientFree(client);

    return 0;
}
