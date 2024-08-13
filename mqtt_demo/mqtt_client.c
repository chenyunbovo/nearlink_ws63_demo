#include "cmsis_os2.h"
#include "app_init.h"
#include <std_def.h>
#include "soc_osal.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "td_base.h"
#include "td_type.h"
#include "MQTTClient.h"
#include "mqtt_client.h"

static MQTTClient client;

#if USE_SSL
static MQTTClient_SSLOptions configureSSLOptions() 
{
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;
    // CA_CERTIFICATE_FILE_Path specify server CA
    // ssl_opts.trustStore = CA_CERTIFICATE_FILE_Path;
    return ssl_opts;
}
#endif

void publish(MQTTClient client, char *topic, char *payload) 
{
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Send `%s` to topic `%s` \n", payload, TOPIC);
}

// Define the on_message callback function to print the content of the messages received by the subscribed topic
static int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) 
{
    UNUSED(context);
    UNUSED(topicLen);
    char *payload = message->payload;
    printf("Received `%s` from `%s` topic \n", payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void mqtt_client_deinit(void)
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}


void mqtt_client_connect(void)
{
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    #if USE_SSL
        MQTTClient_SSLOptions ssl_opts = configureSSLOptions();
        conn_opts.ssl = &ssl_opts;
    #endif

    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect mqtt, return code %d\n", rc);
    } else {
        // subscribe topic
        MQTTClient_subscribe(client, TOPIC, QOS);
        printf("Connected to MQTT Broker!\n");
    }
} 

void mqtt_client_init(void) 
{
    int rc;
    MQTTClient_init_options ops;
    ops.do_openssl_init = 0;
    MQTTClient_global_init(&ops);
    rc = MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to create mqtt client, return code %d\n", rc);
    }
    // 设置MQTT消息接受回调函数
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
}
