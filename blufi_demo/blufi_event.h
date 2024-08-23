#ifndef __BLUFI_EVENT_H__
#define __BLUFI_EVENT_H__
#include <stdint.h>

typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_NAN,       /**< WiFi NAN mode */
    WIFI_MODE_MAX
} wifi_mode_t;

typedef enum {
    ESP_BLUFI_STA_CONN_SUCCESS = 0x00,
    ESP_BLUFI_STA_CONN_FAIL    = 0x01,
    ESP_BLUFI_STA_CONNECTING   = 0x02,
    ESP_BLUFI_STA_NO_IP        = 0x03,
} blufi_sta_conn_state_t;

typedef enum {
    BTC_BLUFI_ACT_INIT = 0,
    BTC_BLUFI_ACT_DEINIT,
    BTC_BLUFI_ACT_SEND_CFG_REPORT,
    BTC_BLUFI_ACT_SEND_WIFI_LIST,
    BTC_BLUFI_ACT_SEND_ERR_INFO,
    BTC_BLUFI_ACT_SEND_CUSTOM_DATA,
} btc_blufi_act_t;

typedef enum {
    ESP_BLUFI_SEQUENCE_ERROR = 0,
    ESP_BLUFI_CHECKSUM_ERROR,
    ESP_BLUFI_DECRYPT_ERROR,
    ESP_BLUFI_ENCRYPT_ERROR,
    ESP_BLUFI_INIT_SECURITY_ERROR,
    ESP_BLUFI_DH_MALLOC_ERROR,
    ESP_BLUFI_DH_PARAM_ERROR,
    ESP_BLUFI_READ_PARAM_ERROR,
    ESP_BLUFI_MAKE_PUBLIC_ERROR,
    ESP_BLUFI_DATA_FORMAT_ERROR,
    ESP_BLUFI_CALC_MD5_ERROR,
    ESP_BLUFI_WIFI_SCAN_FAIL,
    ESP_BLUFI_MSG_STATE_ERROR,
} blufi_error_state_t;

typedef struct {
    //station
    uint8_t sta_bssid[6];           /*!< BSSID of station interface */
    bool sta_bssid_set;             /*!< is BSSID of station interface set */
    uint8_t *sta_ssid;              /*!< SSID of station interface */
    int sta_ssid_len;               /*!< length of SSID of station interface */
    uint8_t *sta_passwd;            /*!< password of station interface */
    int sta_passwd_len;             /*!< length of password of station interface */
    uint8_t *softap_ssid;           /*!< SSID of softap interface */
    int softap_ssid_len;            /*!< length of SSID of softap interface */
    uint8_t *softap_passwd;         /*!< password of station interface */
    int softap_passwd_len;          /*!< length of password of station interface */
    uint8_t softap_authmode;        /*!< authentication mode of softap interface */
    bool softap_authmode_set;       /*!< is authentication mode of softap interface set */
    uint8_t softap_max_conn_num;    /*!< max connection number of softap interface */
    bool softap_max_conn_num_set;   /*!< is max connection number of softap interface set */
    uint8_t softap_channel;         /*!< channel of softap interface */
    bool softap_channel_set;        /*!< is channel of softap interface set */
    uint8_t sta_max_conn_retry;     /*!< max retry of sta establish connection */
    bool sta_max_conn_retry_set;    /*!< is max retry of sta establish connection set */
    uint8_t sta_conn_end_reason;    /*!< reason of sta connection end */
    bool sta_conn_end_reason_set;   /*!< is reason of sta connection end set */
    int8_t sta_conn_rssi;           /*!< rssi of sta connection */
    bool sta_conn_rssi_set;         /*!< is rssi of sta connection set */
} blufi_extra_info_t;

typedef struct {
    uint8_t ssid[33];                     /**< SSID of AP */
    int8_t  rssi;                         /**< signal strength of AP */
} blufi_ap_record_t;

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
} blufi_cb_event_t;

#define BTC_PID_BLUFI 1

typedef union {
    struct blufi_cfg_report {
        wifi_mode_t opmode;
        blufi_sta_conn_state_t sta_conn_state;
        uint8_t softap_conn_num;
        blufi_extra_info_t *extra_info;
        int extra_info_len;
    } wifi_conn_report;
    /*
        BTC_BLUFI_ACT_SEND_WIFI_LIST
    */
    struct blufi_wifi_list {
        uint16_t apCount;
        blufi_ap_record_t *list;
    } wifi_list;
    /*
        BTC_BLUFI_ACT_SEND_ERR_INFO
    */
    struct blufi_error_infor {
        blufi_error_state_t state;
    } blufi_err_infor;
    /*
        BTC_BLUFI_ACT_SEND_CUSTOM_DATA
    */
    struct blufi_custom_data {
         uint8_t *data;
         uint32_t data_len;
    } custom_data;
} btc_blufi_args_t;

#endif  // __BLUFI_EVENT_H__
