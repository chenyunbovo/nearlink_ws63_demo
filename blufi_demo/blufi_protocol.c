#include "lwip/netifapi.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "uapi_crc.h"
#include "blufi_protocol.h"

tBLUFI_ENV blufi_env;

void btc_blufi_send_encap(uint8_t type, uint8_t *data, int total_data_len);

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len)
{
    return uapi_crc16(0, data, len);
}

void btc_blufi_report_error(uint8_t state)
{
    uint8_t hdr[5] = {0x49,0x04,blufi_env.send_seq++,0x01,state};
    ble_uart_server_send_input_report(hdr,5);
}

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

static void btc_blufi_send_ack(uint8_t seq)
{
    uint8_t type;
    uint8_t data;

    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_CTRL, BLUFI_TYPE_CTRL_SUBTYPE_ACK);
    data = seq;

    btc_blufi_send_encap(type, &data, 1);
}

void btc_blufi_recv_handler(uint8_t *data, int len)
{
    struct blufi_hdr *hdr = (struct blufi_hdr *)data;
    uint16_t checksum, checksum_pkt;
    int ret;

    if (hdr->seq != blufi_env.recv_seq) {
        btc_blufi_report_error(ESP_BLUFI_SEQUENCE_ERROR);
        return;
    }

    blufi_env.recv_seq++;

    // first step, decrypt
    if (BLUFI_FC_IS_ENC(hdr->fc)) {
        // ret = blufi_env.cbs->decrypt_func(hdr->seq, hdr->data, hdr->data_len);
        // if (ret != hdr->data_len) { /* enc must be success and enc len must equal to plain len */
        //     printf("%s decrypt error %d\n", __func__, ret);
        //     btc_blufi_report_error(ESP_BLUFI_DECRYPT_ERROR);
        //     return;
        // }
    }

    // second step, check sum
    if (BLUFI_FC_IS_CHECK(hdr->fc)) {
        checksum = blufi_crc_checksum(hdr->seq, &hdr->seq, hdr->data_len + 2);
        checksum_pkt = hdr->data[hdr->data_len] | (((uint16_t) hdr->data[hdr->data_len + 1]) << 8);
        if (checksum != checksum_pkt) {
            btc_blufi_report_error(ESP_BLUFI_CHECKSUM_ERROR);
            return;
        }
    }

    if (BLUFI_FC_IS_REQ_ACK(hdr->fc)) {
        btc_blufi_send_ack(hdr->seq);
    }

    if (BLUFI_FC_IS_FRAG(hdr->fc)) {
        if (blufi_env.offset == 0) {
            if (blufi_env.aggr_buf) {
                printf("%s msg error, blufi_env.aggr_buf is not freed\n", __func__);
                btc_blufi_report_error(ESP_BLUFI_MSG_STATE_ERROR);
                return;
            }
            blufi_env.total_len = hdr->data[0] | (((uint16_t) hdr->data[1]) << 8);
            blufi_env.aggr_buf = osal_vmalloc(blufi_env.total_len);
            if (blufi_env.aggr_buf == NULL) {
                printf("%s no mem, len %d\n", __func__, blufi_env.total_len);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                return;
            }
        }
        if (blufi_env.offset + hdr->data_len  - 2 <= blufi_env.total_len){
            memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data + 2, hdr->data_len  - 2);
            blufi_env.offset += (hdr->data_len - 2);
        } else {
            printf("%s payload is longer than packet length, len %d \n", __func__, blufi_env.total_len);
            btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
            return;
        }

    } else {
        if (blufi_env.offset > 0) {   /* if previous pkt is frag */
            /* blufi_env.aggr_buf should not be NULL */
            if (blufi_env.aggr_buf == NULL) {
                printf("%s buffer is NULL\n", __func__);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                return;
            }
            /* payload length should be equal to total_len */
            if ((blufi_env.offset + hdr->data_len) != blufi_env.total_len) {
                btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
                return;
            }
            memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data, hdr->data_len);

            btc_blufi_protocol_handler(hdr->type, blufi_env.aggr_buf, blufi_env.total_len);
            blufi_env.offset = 0;
            osal_vfree(blufi_env.aggr_buf);
            blufi_env.aggr_buf = NULL;
        } else {
            btc_blufi_protocol_handler(hdr->type, hdr->data, hdr->data_len);
            blufi_env.offset = 0;
        }
    }
}

void btc_blufi_send_encap(uint8_t type, uint8_t *data, int total_data_len)
{
    struct blufi_hdr *hdr = NULL;
    int remain_len = total_data_len;
    uint16_t checksum;
    int ret;

    if (blufi_env.is_connected == false) {
        printf("blufi connection has been disconnected \n");
        return;
    }

    while (remain_len > 0) {
        if (remain_len > blufi_env.frag_size) {
            hdr = osal_vmalloc(sizeof(struct blufi_hdr) + 2 + blufi_env.frag_size + 2);
            if (hdr == NULL) {
                printf("%s no mem\n", __func__);
                return;
            }
            hdr->fc = 0x0;
            hdr->data_len = blufi_env.frag_size + 2;
            hdr->data[0] = remain_len & 0xff;
            hdr->data[1] = (remain_len >> 8) & 0xff;
            memcpy(hdr->data + 2, &data[total_data_len - remain_len], blufi_env.frag_size); //copy first, easy for check sum
            hdr->fc |= BLUFI_FC_FRAG;
        } else {
            hdr = osal_vmalloc(sizeof(struct blufi_hdr) + remain_len + 2);
            if (hdr == NULL) {
                printf("%s no mem\n", __func__);
                return;
            }
            hdr->fc = 0x0;
            hdr->data_len = remain_len;
            memcpy(hdr->data, &data[total_data_len - remain_len], hdr->data_len); //copy first, easy for check sum
        }

        hdr->type = type;
        hdr->fc |= BLUFI_FC_DIR_E2P;
        hdr->seq = blufi_env.send_seq++;

        if (BLUFI_TYPE_IS_CTRL(hdr->type)) {
            if ((blufi_env.sec_mode & BLUFI_CTRL_SEC_MODE_CHECK_MASK)) {
                hdr->fc |= BLUFI_FC_CHECK;
                checksum = blufi_crc_checksum(hdr->seq, &hdr->seq, hdr->data_len + 2);
                memcpy(&hdr->data[hdr->data_len], &checksum, 2);
            }
        } else if (!BLUFI_TYPE_IS_DATA_NEG(hdr->type) && !BLUFI_TYPE_IS_DATA_ERROR_INFO(hdr->type)) {
            if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_CHECK_MASK)) {
                hdr->fc |= BLUFI_FC_CHECK;
                checksum = blufi_crc_checksum(hdr->seq, &hdr->seq, hdr->data_len + 2);
                memcpy(&hdr->data[hdr->data_len], &checksum, 2);
            }

            if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_ENC_MASK)) {
                
            }
        }

        if (hdr->fc & BLUFI_FC_FRAG) {
            remain_len -= (hdr->data_len - 2);
        } else {
            remain_len -= hdr->data_len;
        }

        ble_uart_server_send_input_report((uint8_t *)hdr,((hdr->fc & BLUFI_FC_CHECK) ?
                           hdr->data_len + sizeof(struct blufi_hdr) + 2 :
                           hdr->data_len + sizeof(struct blufi_hdr)));

        osal_vfree(hdr);
        hdr =  NULL;
    }
}

uint16_t btc_blufi_get_version(void)
{
    return BTC_BLUFI_VERSION;
}
