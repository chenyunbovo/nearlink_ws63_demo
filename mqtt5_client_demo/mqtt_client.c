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
    MQTTClient_publishMessage5(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Send `%s` to topic `%s` \n", payload, topic);
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
    enum MQTTReasonCodes reasonCode = 0;
    MQTTProperties props;
    MQTTClient_disconnect5(client, 0, reasonCode, &props);
    MQTTClient_destroy(&client);
}

void mqtt_client_init(void) 
{
    int rc;
    MQTTResponse ret;
    MQTTClient_init_options ops;
    ops.do_openssl_init = 0;
    MQTTClient_global_init(&ops);
    MQTTClient_createOptions create_opts = { {'M', 'Q', 'C', 'O'}, 0, MQTTVERSION_5 };

    rc = MQTTClient_createWithOptions(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL, &create_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to create mqtt client, return code %d\n", rc);
    }
    // 设置MQTT消息接受回调函数
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer5;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    MQTTProperties props = MQTTProperties_initializer;
    MQTTProperties willProps = MQTTProperties_initializer;

    
    #if USE_SSL
        MQTTClient_SSLOptions ssl_opts = configureSSLOptions();
        conn_opts.ssl = &ssl_opts;
    #endif

    ret = MQTTClient_connect5(client, &conn_opts, &props, &willProps);
    if (ret.reasonCode == MQTTREASONCODE_SUCCESS) {
        printf("Connected to MQTT Broker!\n");
    } else {
        printf("failed to connect to MQTT Broker, return code %d\n", (int8_t)ret.reasonCode);
    }
    MQTTResponse_free(ret);
    // subscribe topic
    MQTTSubscribe_options subopts = MQTTSubscribe_options_initializer;

    subopts.retainAsPublished = 1;
    subopts.noLocal = 0;
    subopts.retainHandling = 0;

    ret = MQTTClient_subscribe5(client, TOPIC, QOS, &subopts, &props);
    if (ret.reasonCode == MQTTREASONCODE_SUCCESS) {
        printf("subscribe topic succ!\n");
    } else {
        printf("failed to connect to MQTT Broker, return code %d\n", (int8_t)ret.reasonCode);
    }

    for (int i = 0; i < 5; i++) {
        publish(client,"test_pub","1223121313");
        (void)osDelay(100);
    }
    MQTTResponse_free(ret);
}
