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
#include "blufi_event.h"

tBLUFI_ENV blufi_env;

void btc_blufi_send_encap(uint8_t type, uint8_t *data, int total_data_len);

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len)
{
    unused(iv8);
    return uapi_crc16(0, data, len);
}

void btc_blufi_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    blufi_cb_param_t *dst = (blufi_cb_param_t *) p_dest;
    blufi_cb_param_t *src = (blufi_cb_param_t *) p_src;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        dst->sta.ssid = osal_vmalloc(src->sta.ssid_len);
        if (dst->sta.ssid == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta.ssid, src->sta.ssid, src->sta.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        dst->sta.passwd = osal_vmalloc(src->sta.passwd_len);
        if (dst->sta.passwd == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta.passwd, src->sta.passwd, src->sta.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        dst->softap.ssid = osal_vmalloc(src->softap.ssid_len);
        if (dst->softap.ssid == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap.ssid, src->softap.ssid, src->softap.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        dst->softap.passwd = osal_vmalloc(src->softap.passwd_len);
        if (dst->softap.passwd == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap.passwd, src->softap.passwd, src->softap.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        dst->ca.name = osal_vmalloc(src->ca.name_len);
        if (dst->ca.name == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.name, src->ca.name, src->ca.name_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        dst->ca.cert = osal_vmalloc(src->ca.cert_len);
        if (dst->ca.cert == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.cert, src->ca.cert, src->ca.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        dst->ca.client_cert = osal_vmalloc(src->ca.client_cert_len);
        if (dst->ca.client_cert == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.client_cert, src->ca.client_cert, src->ca.client_cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        dst->ca.server_cert = osal_vmalloc(src->ca.server_cert_len);
        if (dst->ca.server_cert == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.server_cert, src->ca.server_cert, src->ca.server_cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        dst->ca.client_pkey = osal_vmalloc(src->ca.client_pkey_len);
        if (dst->ca.client_pkey == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.client_pkey, src->ca.client_pkey, src->ca.client_pkey_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        dst->ca.server_pkey = osal_vmalloc(src->ca.server_pkey_len);
        if (dst->ca.server_pkey == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.server_pkey, src->ca.server_pkey, src->ca.server_pkey_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        dst->custom_data = osal_vmalloc(src->custom_data_len);
        if (dst->custom_data == NULL) {
            printf("%s %d no mem\n", __func__, msg->act);
            break;
        }
        memcpy(dst->custom_data, src->custom_data, src->custom_data_len);
        break;
    default:
        break;
    }
}

void btc_blufi_cb_deep_free(btc_msg_t *msg)
{
    blufi_cb_param_t *param = (blufi_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        osal_vfree(param->sta.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        osal_vfree(param->sta.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        osal_vfree(param->softap.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        osal_vfree(param->softap.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        osal_vfree(param->ca.name);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        osal_vfree(param->ca.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        osal_vfree(param->ca.client_cert);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        osal_vfree(param->ca.server_cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        osal_vfree(param->ca.client_pkey);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        osal_vfree(param->ca.server_pkey);
        break;
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        osal_vfree(param->custom_data);
        break;
    default:
        break;
    }
}

void btc_blufi_report_error(uint8_t state)
{
    uint8_t hdr[5] = {0x49,0x04,blufi_env.send_seq++,0x01,state};
    ble_uart_server_send_input_report(hdr,5);
}

int btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func,
                                    btc_arg_deep_free_t free_func)
{
    btc_msg_t* lmsg;
    if ((msg == NULL) || ((arg == NULL) == !(arg_len == 0))) {
        printf("%s Invalid parameters\n", __func__);
        return -1;
    }

    printf("%s msg %u %u %p\n", __func__, msg->pid, msg->act, arg);

    lmsg = (btc_msg_t *)osal_vmalloc(sizeof(btc_msg_t) + arg_len);
    if (lmsg == NULL) {
        printf("%s No memory\n", __func__);
        return -2;
    }

    memcpy(lmsg, msg, sizeof(btc_msg_t));
    if (arg) {
        memset(lmsg->arg, 0x00, arg_len);    //important, avoid arg which have no length
        memcpy(lmsg->arg, arg, arg_len);
        if (copy_func) {
            copy_func(lmsg, lmsg->arg, arg);
        }
    }
    
    if (copy_func && free_func) {
        free_func(lmsg);
    }
    osal_vfree(lmsg);

    return 0;
}

void btc_blufi_protocol_handler(uint8_t type, uint8_t *data, int len)
{
    btc_msg_t msg;
    uint8_t *output_data = NULL;
    blufi_cb_param_t param;
    int output_len = 0;

    switch (BLUFI_GET_TYPE(type)) {
    case BLUFI_TYPE_CTRL:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_CTRL_SUBTYPE_ACK:
            /* TODO: check sequence */
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_SEC_MODE:
            blufi_env.sec_mode = data[0];
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_SET_WIFI_OPMODE;
            param.wifi_mode = data[0];

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), NULL, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP;

            btc_transfer_context(&msg, NULL, 0, NULL, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP;

            btc_transfer_context(&msg, NULL, 0, NULL, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_GET_WIFI_STATUS;

            btc_transfer_context(&msg, NULL, 0, NULL, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_DEAUTHENTICATE_STA;

            btc_transfer_context(&msg, NULL, 0, NULL,NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_VERSION: {
            uint8_t type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_REPLY_VERSION);
            uint8_t data[2];

            data[0] = BTC_BLUFI_GREAT_VER;
            data[1] = BTC_BLUFI_SUB_VER;
            btc_blufi_send_encap(type, &data[0], sizeof(data));
            break;
        }
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONNECT_BLE:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE;
            btc_transfer_context(&msg, NULL, 0, NULL, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_LIST:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_GET_WIFI_LIST;
            btc_transfer_context(&msg, NULL, 0, NULL, NULL);
            break;
        default:
            printf("%s Unkown Ctrl pkt %02x\n", __func__, type);
            break;
        }
        break;
    case BLUFI_TYPE_DATA:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_DATA_SUBTYPE_NEG:
            if (output_data && output_len > 0) {
                btc_blufi_send_encap(BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_NEG),
                             output_data, output_len);
            }
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_BSSID;
            memcpy(param.sta.bssid, &data[0], 6);

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), NULL, NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_SSID:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_SSID;
            param.sta.ssid = &data[0];
            param.sta.ssid_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_PASSWD;
            param.sta.passwd = &data[0];
            param.sta.passwd_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_SSID;
            param.softap.ssid = &data[0];
            param.softap.ssid_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD;
            param.softap.passwd = &data[0];
            param.softap.passwd_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM;
            param.softap.max_conn_num = data[0];

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), NULL, NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE;
            param.softap.auth_mode = data[0];

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), NULL, NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL;
            param.softap.channel = data[0];

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), NULL, NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_USERNAME:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_USERNAME;
            param.ca.name = &data[0];
            param.ca.name_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CA:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CA_CERT;
            param.ca.cert = &data[0];
            param.ca.cert_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CLIENT_CERT;
            param.ca.client_pkey = &data[0];
            param.ca.client_pkey_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SERVER_CERT;
            param.ca.server_cert = &data[0];
            param.ca.server_cert_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY;
            param.ca.client_pkey = &data[0];
            param.ca.client_pkey_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY;
            param.ca.server_pkey = &data[0];
            param.ca.server_pkey_len = len;

            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CUSTOM_DATA:
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CUSTOM_DATA;
            param.custom_data = &data[0];
            param.custom_data_len = len;
            btc_transfer_context(&msg, &param, sizeof(blufi_cb_param_t), btc_blufi_cb_deep_copy, btc_blufi_cb_deep_free);
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
    unused(len);
    struct blufi_hdr *hdr = (struct blufi_hdr *)data;
    uint16_t checksum, checksum_pkt;

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
