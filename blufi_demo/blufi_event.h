#ifndef __BLUFI_EVENT_H__
#define __BLUFI_EVENT_H__

typedef struct btc_msg {
    uint8_t aid;    //application id
    uint8_t pid;    //profile id
    uint8_t act;    //profile action, defined in seprerate header files
    uint8_t   arg[0]; //param for btc function or function param
} btc_msg_t;

typedef void (* btc_arg_deep_copy_t)(btc_msg_t *msg, void *dst, void *src);
typedef void (* btc_arg_deep_free_t)(btc_msg_t *msg);

typedef enum {
    ESP_BLUFI_EVENT_INIT_FINISH = 0,                     /*<! When BLUFI init complete, this event happen */
    ESP_BLUFI_EVENT_DEINIT_FINISH,                       /*<! When BLUFI deinit complete, this event happen */
    ESP_BLUFI_EVENT_SET_WIFI_OPMODE,                     /*<! When Phone set ESP32 wifi operation mode(AP/STA/AP_STA), this event happen */
    ESP_BLUFI_EVENT_BLE_CONNECT,                         /*<! When Phone connect to ESP32 with BLE, this event happen */
    ESP_BLUFI_EVENT_BLE_DISCONNECT,                      /*<! When Phone disconnect with BLE, this event happen */
    ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP,                   /*<! When Phone request ESP32's STA connect to AP, this event happen */
    ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP,              /*<! When Phone request ESP32's STA disconnect from AP, this event happen */
    ESP_BLUFI_EVENT_GET_WIFI_STATUS,                     /*<! When Phone get ESP32 wifi status, this event happen */
    ESP_BLUFI_EVENT_DEAUTHENTICATE_STA,                  /*<! When Phone deauthenticate sta from SOFTAP, this event happen */
    /* recv data */
    ESP_BLUFI_EVENT_RECV_STA_BSSID,                      /*<! When Phone send STA BSSID to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_STA_SSID,                       /*<! When Phone send STA SSID to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_STA_PASSWD,                     /*<! When Phone send STA PASSWORD to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_SSID,                    /*<! When Phone send SOFTAP SSID to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD,                  /*<! When Phone send SOFTAP PASSWORD to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM,            /*<! When Phone send SOFTAP max connection number to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE,               /*<! When Phone send SOFTAP authentication mode to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL,                 /*<! When Phone send SOFTAP channel to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_USERNAME,                       /*<! When Phone send username to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CA_CERT,                        /*<! When Phone send CA certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CLIENT_CERT,                    /*<! When Phone send Client certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SERVER_CERT,                    /*<! When Phone send Server certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY,                /*<! When Phone send Client Private key to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY,                /*<! When Phone send Server Private key to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE,           /*<! When Phone send Disconnect key to ESP32, this event happen */
    ESP_BLUFI_EVENT_GET_WIFI_LIST,                       /*<! When Phone send get wifi list command to ESP32, this event happen */
    ESP_BLUFI_EVENT_REPORT_ERROR,                        /*<! When Blufi report error, this event happen */
    ESP_BLUFI_EVENT_RECV_CUSTOM_DATA,                    /*<! When Phone send custom data to ESP32, this event happen */
} esp_blufi_cb_event_t;

#define BTC_PID_BLUFI 1

#endif  // __BLUFI_EVENT_H__
