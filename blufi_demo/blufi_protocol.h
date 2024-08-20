#ifndef __BLUFI_PROTOCOL__
#define __BLUFI_PROTOCOL__

#define BTC_BLUFI_GREAT_VER   0x01  //Version + Subversion
#define BTC_BLUFI_SUB_VER     0x03  //Version + Subversion
#define BTC_BLUFI_VERSION     ((BTC_BLUFI_GREAT_VER<<8)|BTC_BLUFI_SUB_VER)  //Version + Subversion

typedef uint8_t tGATT_IF;
/* service engine control block */
typedef struct {
    /* Protocol reference */
    tGATT_IF                  gatt_if;
    uint8_t                   srvc_inst;
    uint16_t                  handle_srvc;
    uint16_t                  handle_char_p2e;
    uint16_t                  handle_char_e2p;
    uint16_t                  handle_descr_e2p;
    uint16_t                  conn_id;
    bool                      is_connected;
    uint32_t                  trans_id;
    uint8_t                   congest;
    uint16_t                  frag_size;
#define BLUFI_PREPAIR_BUF_MAX_SIZE 1024
    uint8_t                  *prepare_buf;
    int                       prepare_len;
    bool                      enabled;
    uint8_t                   send_seq;
    uint8_t                   recv_seq;
    uint8_t                   sec_mode;
    uint8_t                  *aggr_buf;
    uint16_t                  total_len;
    uint16_t                  offset;
} tBLUFI_ENV;

/* BLUFI protocol */
struct blufi_hdr{
    uint8_t type;
    uint8_t fc;
    uint8_t seq;
    uint8_t data_len;
    uint8_t data[0];
};
typedef struct blufi_hdr blufi_hd_t;

struct blufi_frag_hdr {
    uint8_t type;
    uint8_t fc;
    uint8_t seq;
    uint8_t data_len;
    uint16_t total_len;
    uint8_t content[0];
};
typedef struct blufi_frag_hdr blufi_frag_hdr_t;

// packet type
#define BLUFI_TYPE_MASK         0x03
#define BLUFI_TYPE_SHIFT        0
#define BLUFI_SUBTYPE_MASK      0xFC
#define BLUFI_SUBTYPE_SHIFT     2

#define BLUFI_GET_TYPE(type)    ((type) & BLUFI_TYPE_MASK)
#define BLUFI_GET_SUBTYPE(type) (((type) & BLUFI_SUBTYPE_MASK) >>BLUFI_SUBTYPE_SHIFT)
#define BLUFI_BUILD_TYPE(type, subtype) (((type) & BLUFI_TYPE_MASK) | ((subtype)<<BLUFI_SUBTYPE_SHIFT))

#define BLUFI_TYPE_CTRL                                 0x0
#define BLUFI_TYPE_CTRL_SUBTYPE_ACK                     0x00
#define BLUFI_TYPE_CTRL_SUBTYPE_SET_SEC_MODE            0x01
#define BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE         0x02
#define BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP              0x03
#define BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP         0x04
#define BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS         0x05
#define BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA      0x06
#define BLUFI_TYPE_CTRL_SUBTYPE_GET_VERSION             0x07
#define BLUFI_TYPE_CTRL_SUBTYPE_DISCONNECT_BLE          0x08
#define BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_LIST           0x09

#define BLUFI_TYPE_DATA                                 0x1
#define BLUFI_TYPE_DATA_SUBTYPE_NEG                     0x00
#define BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID               0x01
#define BLUFI_TYPE_DATA_SUBTYPE_STA_SSID                0x02
#define BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD              0x03
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID             0x04
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD           0x05
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM     0x06
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE        0x07
#define BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL          0x08
#define BLUFI_TYPE_DATA_SUBTYPE_USERNAME                0x09
#define BLUFI_TYPE_DATA_SUBTYPE_CA                      0x0a
#define BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT             0x0b
#define BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT             0x0c
#define BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY         0x0d
#define BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY         0x0e
#define BLUFI_TYPE_DATA_SUBTYPE_WIFI_REP                0x0f
#define BLUFI_TYPE_DATA_SUBTYPE_REPLY_VERSION           0x10
#define BLUFI_TYPE_DATA_SUBTYPE_WIFI_LIST               0x11
#define BLUFI_TYPE_DATA_SUBTYPE_ERROR_INFO              0x12
#define BLUFI_TYPE_DATA_SUBTYPE_CUSTOM_DATA             0x13
#define BLUFI_TYPE_DATA_SUBTYPE_STA_MAX_CONN_RETRY      0x14
#define BLUFI_TYPE_DATA_SUBTYPE_STA_CONN_END_REASON     0x15
#define BLUFI_TYPE_DATA_SUBTYPE_STA_CONN_RSSI           0x16

/* Service UUID */
#define BLUFI_UUID_SERVER_SERVICE 0xFFFF
/* Characteristic UUID */
#define BLUFI_CHARACTERISTIC_UUID_TX 0xFF02
/* Characteristic UUID */
#define BLUFI_CHARACTERISTIC_UUID_RX 0xFF01
/* Client Characteristic Configuration UUID */
#define BLUFI_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902

// packet frame control
#define BLUFI_FC_ENC_MASK       0x01
#define BLUFI_FC_CHECK_MASK     0x02
#define BLUFI_FC_DIR_MASK       0x04
#define BLUFI_FC_REQ_ACK_MASK   0x08
#define BLUFI_FC_FRAG_MASK      0x10

#define BLUFI_FC_ENC            0x01
#define BLUFI_FC_CHECK          0x02
#define BLUFI_FC_DIR_P2E        0x00
#define BLUFI_FC_DIR_E2P        0x04
#define BLUFI_FC_REQ_ACK        0x08
#define BLUFI_FC_FRAG           0x10

#define BLUFI_FC_IS_ENC(fc)       ((fc) & BLUFI_FC_ENC_MASK)
#define BLUFI_FC_IS_CHECK(fc)     ((fc) & BLUFI_FC_CHECK_MASK)
#define BLUFI_FC_IS_DIR_P2E(fc)   ((fc) & BLUFI_FC_DIR_P2E_MASK)
#define BLUFI_FC_IS_DIR_E2P(fc)   (!((fc) & BLUFI_DIR_P2E_MASK))
#define BLUFI_FC_IS_REQ_ACK(fc)   ((fc) & BLUFI_FC_REQ_ACK_MASK)
#define BLUFI_FC_IS_FRAG(fc)      ((fc) & BLUFI_FC_FRAG_MASK)


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
} esp_blufi_error_state_t;

errcode_t ble_uart_server_send_input_report(uint8_t *data, uint16_t len);

#endif // __BLUFI_PROTOCOL__