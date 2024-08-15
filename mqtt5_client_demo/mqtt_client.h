#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

// Enable or disable SSL/TLS connection (1 for SSL/TLS, 0 for TCP)
#include "MQTTClient.h"
#define USE_SSL 0

#if USE_SSL
#define ADDRESS "ssl://broker.emqx.io:8883"
#else
#define ADDRESS "tcp://192.168.137.1:1883"
#endif

#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client"
#define QOS         0
#define TOPIC       "emqx/c-test"
#define TIMEOUT     10000L

void publish(MQTTClient client, char *topic, char *payload);
void mqtt_client_deinit(void);
void mqtt_client_connect(void);
void mqtt_client_init(void);
#endif
