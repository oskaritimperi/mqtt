#include <stdlib.h>
#include <stdio.h>

#include "mqtt.h"
#include "getopt.h"

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

void onSubscribe(MqttClient *client, int id, MqttSubscriptionStatus status)
{
    (void) client;
    printf("onSubscribe id=%d status=%d\n", id, status);
}

void onMessage(MqttClient *client, const char *topic, const void *data,
    size_t size)
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

int main(int argc, char **argv)
{
    MqttClient *client;
    const char *opt;
    struct options options;

    options.qos = 0;
    options.topic = "$SYS/broker/load/messages/#";
    options.clean = 1;
    options.client_id = NULL;

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

            GETOPT_OPTARG("--topic"):
                options.topic = optarg;
                break;

            GETOPT_OPT("--no-clean"):
                options.clean = 0;
                break;

            GETOPT_OPTARG("--id"):
                options.client_id = optarg;
                break;

            GETOPT_MISSING_ARG:
                fprintf(stderr, "missing argument to: %s\n", opt);

            GETOPT_DEFAULT:
                usage(argv[0]);
                break;
        }
    }

    client = MqttClientNew(options.client_id, options.clean);

    MqttClientSetOnConnect(client, onConnect);
    MqttClientSetOnSubscribe(client, onSubscribe);
    MqttClientSetOnMessage(client, onMessage);
    MqttClientSetUserData(client, &options);

    MqttClientConnect(client, "test.mosquitto.org", 1883, 60);

    MqttClientRun(client);

    MqttClientFree(client);

    return 0;
}
