/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "osal_addr.h"
#include "osal_debug.h"
#include "product.h"
#include "securec.h"
#include "errcode.h"
#include "uart.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "stdlib.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "reboot_porting.h"
#include "wifi_device.h"
#include "blufi_protocol.h"
#include "blufi_adv.h"

#define BLUFI_STA_SAMPLE_LOG              "[BLUFI]"

#define BLUFI_TASK_PRIO                  (osPriority_t)(13)
#define BLUFI_TASK_STACK_SIZE            0x1000


/* uart gatt server id */
#define BLUFI_SERVER_ID 			1
/* uart ble connect id */
#define BLE_SINGLE_LINK_CONNECT_ID 	1
/* octets of 16 bits uart */
#define UART16_LEN 					2
/* invalid attribute handle */
#define INVALID_ATT_HDL 			0
/* invalid server ID */
#define INVALID_SERVER_ID 			0

#define BLUFI_SERVICE_NUM 3

static uint16_t g_blufi_conn_id;
static uint8_t g_blufi_name_value[] = { 'b', 'l', 'e', '_', 'u', 'a', 'r', 't', '\0' };
static uint8_t g_uart_server_app_uuid[] = { 0x00, 0x00 };
static uint8_t g_blufi_server_addr[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
static uint8_t g_server_id = INVALID_SERVER_ID;
static uint8_t g_connection_state = 0;
static uint16_t g_notify_indicate_handle = 16;
static uint8_t g_service_num = 0;
static uint8_t notifycation = 0;

/* device向host发送数据：input report */
errcode_t ble_uart_server_send_input_report(uint8_t *data, uint16_t len)
{
    if (notifycation == 0) {
        return ERRCODE_BT_FAIL;
    }
    gatts_ntf_ind_t param = { 0 };
    uint16_t conn_id = g_blufi_conn_id;
    param.attr_handle = g_notify_indicate_handle;
    param.value_len = len;
    param.value = data;
    gatts_notify_indicate(BLUFI_SERVER_ID, conn_id, &param);
    return ERRCODE_BT_SUCCESS;
}

uint8_t ble_uart_get_connection_state(void)
{
    return g_connection_state;
}

/* 将uint16的uuid数字转化为bt_uuid_t */
static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = UART16_LEN;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

/* 设置注册服务时的name */
void blufi_set_device_name_value(const uint8_t *name, const uint8_t len)
{
    size_t len_name = sizeof(g_blufi_name_value);
    if (memcpy_s(g_blufi_name_value, len_name, name, len) != EOK) {
        osal_printk("%s memcpy name fail\n", BLUFI_STA_SAMPLE_LOG);
    }
}

/* 创建服务 */
static void blufi_add_service(void)
{
    bt_uuid_t uart_service_uuid = { 0 };
    bts_data_to_uuid_len2(BLUFI_UUID_SERVER_SERVICE, &uart_service_uuid);
    gatts_add_service(BLUFI_SERVER_ID, &uart_service_uuid, true);
}

/* 添加uart发送服务的所有特征和描述符 */
static void blufi_add_tx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    osal_printk("%s TX characters:%d srv_handle:%d \n", BLUFI_STA_SAMPLE_LOG, server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x12, 0x34 };
    bts_data_to_uuid_len2(BLUFI_CHARACTERISTIC_UUID_TX, &characters_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_NOTIFY | GATT_CHARACTER_PROPERTY_BIT_READ;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    osal_printk("%s characters_uuid:%2x %2x\n", BLUFI_STA_SAMPLE_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);

    static uint8_t ccc_val[] = { 0x01, 0x00 }; // notify
    bt_uuid_t ccc_uuid = { 0 };
    bts_data_to_uuid_len2(BLUFI_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE |
        GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_val);
    descriptor.value = ccc_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
    osal_printk("%s ccc_uuid:%2x %2x\n", BLUFI_STA_SAMPLE_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);
}

/* 添加uart接收服务的所有特征和描述符 */
static void blufi_add_rx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    osal_printk("%s RX characters:%d srv_handle: %d \n", BLUFI_STA_SAMPLE_LOG, server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x12, 0x34 };
    bts_data_to_uuid_len2(BLUFI_CHARACTERISTIC_UUID_RX, &characters_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_WRITE;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    osal_printk("%s characters_uuid:%2x %2x\n", BLUFI_STA_SAMPLE_LOG, characters_uuid.uuid[0], characters_uuid.uuid[1]);
}

bool bts_uart_compare_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return false;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return false;
    }
    return true;
}

/* 服务添加回调 */
static void blufi_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    osal_printk("%s add characters_and_descriptors cbk service:%d, srv_handle:%d, uuid_len:%d, status:%d, uuid:",
                BLUFI_STA_SAMPLE_LOG, server_id, handle, uuid->uuid_len, status);
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x ", uuid->uuid[i]);
    }
    osal_printk("\n");
    blufi_add_tx_characters_and_descriptors(server_id, handle);
    blufi_add_rx_characters_and_descriptors(server_id, handle);

    gatts_start_service(server_id, handle);
}

/* 特征添加回调 */
static void blufi_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
                                                   gatts_add_character_result_t *result, errcode_t status)
{
    UNUSED(status);
    osal_printk("%s add character cbk service:%d service_hdl: %d char_hdl: %d char_val_hdl: %d uuid_len: %d \n",
                BLUFI_STA_SAMPLE_LOG, server_id, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    osal_printk("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x ", uuid->uuid[i]);
    }
    osal_printk("\n");
}

/* 描述符添加回调 */
static void blufi_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
                                               uint16_t handle, errcode_t status)
{
    UNUSED(status);
    osal_printk("%s service:%d service_hdl: %d desc_hdl: %d uuid_len: %d \n",
                BLUFI_STA_SAMPLE_LOG, server_id, service_handle, handle, uuid->uuid_len);
    osal_printk("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len; i++) {
        osal_printk("%02x ", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
}

/* 开始服务回调 */
static void blufi_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    g_service_num++;
    if ((g_service_num == BLUFI_SERVICE_NUM) && (status == 0)) {
        osal_printk("%s start service cbk , start adv\n", BLUFI_STA_SAMPLE_LOG);
        blufi_set_adv_data();
        blufi_start_adv();
    }
    osal_printk("%s start service:%2d service_hdl: %d status: %d\n",
                BLUFI_STA_SAMPLE_LOG, server_id, handle, status);
}

static void blufi_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    osal_printk("%s blufi write cbk server_id:%d, conn_id:%d, status:%d\n", BLUFI_STA_SAMPLE_LOG, server_id, conn_id, status);
    osal_printk("%s blufi write cbk att_handle:%d\n", BLUFI_STA_SAMPLE_LOG, write_cb_para->handle);
    if ((write_cb_para->length > 0) && write_cb_para->value) {
        switch (write_cb_para->handle) {
        case NOTIFY_HANDLE:
            if (write_cb_para->value[0] == 0x01) {
                notifycation = 1;
            } else {
                notifycation = 0;
            }
            break;
        case WRITE_HANDLE:
            printf("recv data:");
            for (int i = 0; i < write_cb_para->length; i++) {
                printf("%02X ", write_cb_para->value[i]);
            }
            printf("\n");
            break;
        default:
            break;
        }
    }
}

static void blufi_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("%s ReceiveReadReq--server_id:%d conn_id:%d\n", BLUFI_STA_SAMPLE_LOG, server_id, conn_id);
    osal_printk("%s request_id:%d, att_handle:%d offset:%d, need_rsp:%d, is_long:%d\n",
                BLUFI_STA_SAMPLE_LOG, read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset,
                read_cb_para->need_rsp, read_cb_para->is_long);
    osal_printk("%s status:%d\n", BLUFI_STA_SAMPLE_LOG, status);
}

static void blufi_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("%s MtuChanged--server_id:%d conn_id:%d\n", BLUFI_STA_SAMPLE_LOG, server_id, conn_id);
    osal_printk("%s mtusize:%d, status:%d\n", BLUFI_STA_SAMPLE_LOG, mtu_size, status);
}

static void blufi_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv enable cbk adv_id:%d status:%d\n", BLUFI_STA_SAMPLE_LOG, adv_id, status);
}

static void blufi_server_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv disable adv_id: %d, status:%d\n", BLUFI_STA_SAMPLE_LOG, adv_id, status);
}

void blufi_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_blufi_conn_id = conn_id;
    g_connection_state = (uint8_t)conn_state;
    osal_printk("%s connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
                BLUFI_STA_SAMPLE_LOG, conn_id, conn_state, pair_state, addr[0], disc_reason);
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        return;
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        blufi_set_adv_data();
        blufi_start_adv();
    }
}
void blufi_server_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair result conn_id: %d, status: %d, addr %x \n",
                BLUFI_STA_SAMPLE_LOG, conn_id, status, addr[0]);
}

static errcode_t blufi_server_register_callbacks(void)
{
    gap_ble_callbacks_t gap_cb = { 0 };
    gatts_callbacks_t service_cb = { 0 };

    gap_cb.start_adv_cb = blufi_server_adv_enable_cbk;
    gap_cb.conn_state_change_cb = blufi_server_connect_change_cbk;
    gap_cb.stop_adv_cb = blufi_server_adv_disable_cbk;
    gap_cb.pair_result_cb = blufi_server_pair_result_cb;
    errcode_t ret = gap_ble_register_callbacks(&gap_cb);

    service_cb.add_service_cb = blufi_server_service_add_cbk;
    service_cb.add_characteristic_cb = blufi_server_characteristic_add_cbk;
    service_cb.add_descriptor_cb = blufi_server_descriptor_add_cbk;
    service_cb.start_service_cb = blufi_server_service_start_cbk;
    service_cb.read_request_cb = blufi_receive_read_req_cbk;
    service_cb.write_request_cb = blufi_receive_write_req_cbk;
    service_cb.mtu_changed_cb = blufi_mtu_changed_cbk;
    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s reg service cbk failed ret = %d\n", BLUFI_STA_SAMPLE_LOG, ret);
        return ret;
    }
    enable_ble();
    return ret;
}

int blufi_init(void *param)
{
    param = param;
    //  (void)osal_msleep(5000); /* 延时5s，等待SLE初始化完毕 */

    blufi_server_register_callbacks();
    enable_ble();

    errcode_t ret = 0;
    bt_uuid_t app_uuid = { 0 };
    bd_addr_t ble_addr = { 0 };
    app_uuid.uuid_len = sizeof(g_uart_server_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_uart_server_app_uuid, sizeof(g_uart_server_app_uuid)) != EOK) {
        osal_printk("%s add server app uuid memcpy failed\n", BLUFI_STA_SAMPLE_LOG);
        return -1;
    }
    ble_addr.type = BLE_PUBLIC_DEVICE_ADDRESS;
    wifi_get_base_mac_addr((int8_t *)g_blufi_server_addr,6);
    if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_blufi_server_addr, sizeof(g_blufi_server_addr)) != EOK) {
        osal_printk("%s add server app addr memcpy failed\n", BLUFI_STA_SAMPLE_LOG);
        return -1;
    }
    gap_ble_set_local_name(g_blufi_name_value, sizeof(g_blufi_name_value));
    gap_ble_set_local_addr(&ble_addr);
    ret = gatts_register_server(&app_uuid, &g_server_id);
    if ((ret != ERRCODE_BT_SUCCESS) || (g_server_id == INVALID_SERVER_ID)) {
        osal_printk("%s add server failed\r\n", BLUFI_STA_SAMPLE_LOG);
        return -1;
    }
    blufi_add_service(); /* 添加uart服务 */
    osal_printk("%s beginning add service\r\n", BLUFI_STA_SAMPLE_LOG);
    bth_ota_init();
    return 0;
}

static void blufi(void)
{
    osThreadAttr_t attr;
    attr.name       = "blufi_task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BLUFI_TASK_STACK_SIZE;
    attr.priority   = BLUFI_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)blufi_init, NULL, &attr) == NULL) {
        osal_printk("%s::Create blufi_task fail.\r\n", BLUFI_STA_SAMPLE_LOG);
    } else {
        osal_printk("%s::Create blufi_task succ.\r\n", BLUFI_STA_SAMPLE_LOG);
    }
}

app_run(blufi);