#include "lwip/netifapi.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "blufi_protocol.h"

void btc_blufi_protocol_handler(uint8_t type, uint8_t *data, int len)
{
    UNUSED(data);
    UNUSED(len);
    switch (BLUFI_GET_TYPE(type)) {
    case BLUFI_TYPE_CTRL:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_CTRL_SUBTYPE_ACK:
            /* TODO: check sequence */
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_SEC_MODE:
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE:

            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP:

            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP:

            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS:

            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA:

            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_VERSION: {
            break;
        }
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONNECT_BLE:
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_LIST:
            break;
        default:
            printf("%s Unkown Ctrl pkt %02x\n", __func__, type);
            break;
        }
        break;
    case BLUFI_TYPE_DATA:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_DATA_SUBTYPE_NEG:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID:
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_SSID:
           
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE:

            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL:

            break;
        case BLUFI_TYPE_DATA_SUBTYPE_USERNAME:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CA:
            
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT:
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT:
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY:
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY:
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CUSTOM_DATA:

            break;
        default:
            printf("%s Unkown Ctrl pkt %02x\n", __func__, type);
            break;
        }
        break;
    default:
        break;
    }
}
