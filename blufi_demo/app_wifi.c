/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Application core main function for standard \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "wifi_device.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"

#define WIFI_IFNAME_MAX_SIZE             16
#define WIFI_MAX_SSID_LEN                33
#define WIFI_SCAN_AP_LIMIT               64
#define WIFI_MAC_LEN                     6
#define WIFI_STA_SAMPLE_LOG              "[WIFI_STA_SAMPLE]"
#define WIFI_NOT_AVALLIABLE              0
#define WIFI_AVALIABE                    1
#define WIFI_GET_IP_MAX_COUNT            300

#define WIFI_TASK_PRIO                  (osPriority_t)(13)
#define WIFI_TASK_DURATION_MS           2000
#define WIFI_TASK_STACK_SIZE            0x1000
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size);
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code);

wifi_event_stru wifi_event_cb = {
    .wifi_event_connection_changed      = wifi_connection_changed,
    .wifi_event_scan_state_changed      = wifi_scan_state_changed,
};

enum {
    WIFI_STA_SAMPLE_INIT = 0,       /* 0:初始态 */
    WIFI_STA_SAMPLE_SCAN_DONE,      /* 1:扫描完成 */
    WIFI_STA_SAMPLE_CONNECTING,     /* 2:连接中 */
    WIFI_STA_SAMPLE_CONNECT_DONE,   /* 3:关联成功 */
    WIFI_STA_SAMPLE_DISCONNECT,     /* 4:断开连接 */
    WIFI_STA_SAMPLE_GOT_IP,         /* 5:获取IP */
} wifi_state_enum;

td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* 创建的STA接口名 */
wifi_sta_config_stru expected_bss = {0}; /* 连接请求信息 */
struct netif *netif_p = TD_NULL;

uint8_t g_connect = 0;

static void wifi_state_event_cb(td_s32 event, td_s32 *param);

/*****************************************************************************
  STA 扫描事件回调函数
*****************************************************************************/
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    PRINT("%s::Scan done!.\r\n", WIFI_STA_SAMPLE_LOG);
    wifi_state_event_cb(WIFI_STA_SAMPLE_SCAN_DONE, &size);
    return;
}

/*****************************************************************************
  STA 关联事件回调函数
*****************************************************************************/
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(info);
    UNUSED(reason_code);

    if (state == WIFI_NOT_AVALLIABLE) {
        PRINT("%s::Connect fail!. try agin !\r\n", WIFI_STA_SAMPLE_LOG);
        wifi_state_event_cb(WIFI_STA_SAMPLE_DISCONNECT, NULL);
    } else {
        PRINT("%s::Connect succ!.\r\n", WIFI_STA_SAMPLE_LOG);
        wifi_state_event_cb(WIFI_STA_SAMPLE_CONNECT_DONE, NULL);
    }
}

/*****************************************************************************
  STA 匹配目标AP
*****************************************************************************/
td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss, td_u32 num)
{
    td_s32  ret;
    td_bool find_ap = TD_FALSE;
    td_u8   bss_index;
    /* 获取扫描结果 */
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == TD_NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }
    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    for (bss_index = 0; bss_index < num; bss_index ++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    /* 未找到待连接AP,可以继续尝试扫描或者退出 */
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, expected_ssid, strlen(expected_ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_SSID_LEN, key, strlen(key)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1; /* 1：IP类型为动态DHCP获取 */
    osal_kfree(result);
    return 0;
}

/*****************************************************************************
  STA DHCP状态查询
*****************************************************************************/
int example_check_dhcp_status(void *param)
{
    UNUSED(param);
    static td_u8 wait_count = 0;
    while (1) {
        if ((ip_addr_isany(&(netif_p->ip_addr)) == 0) && (wait_count <= WIFI_GET_IP_MAX_COUNT)) {
            /* DHCP成功 */
            PRINT("%s::STA DHCP success.\r\n", WIFI_STA_SAMPLE_LOG);
            wifi_state_event_cb(WIFI_STA_SAMPLE_GOT_IP, NULL);
            break;
        }
        if (wait_count > WIFI_GET_IP_MAX_COUNT) {
            PRINT("%s::STA DHCP timeout, try again !.\r\n", WIFI_STA_SAMPLE_LOG);
            wifi_state_event_cb(WIFI_STA_SAMPLE_DISCONNECT, NULL);
            return -1;
        }
        (void)osDelay(10);
        wait_count ++;
    }
    return 0;
}

static void wifi_state_event_cb(td_s32 event, td_s32 *param)
{
    switch (event) {
    case WIFI_STA_SAMPLE_INIT:
        wifi_sta_scan();
        break;
    case WIFI_STA_SAMPLE_SCAN_DONE:
        if (example_get_match_network(&expected_bss, param[0]) != 0) {
            PRINT("%s::Do not find AP, try again !\r\n", WIFI_STA_SAMPLE_LOG);
            wifi_sta_scan_result_clear();
            wifi_sta_scan();
            break;
        }
        PRINT("%s::Find AP, try to connect.\r\n", WIFI_STA_SAMPLE_LOG);
        /* 启动连接 */
        if (wifi_sta_connect(&expected_bss) != 0) {
            PRINT("%s::wifi_sta_connect fail!!\r\n", WIFI_STA_SAMPLE_LOG);
            wifi_state_event_cb(WIFI_STA_SAMPLE_DISCONNECT, NULL);
        }
        break;  
    case WIFI_STA_SAMPLE_CONNECTING:
        break;
    case WIFI_STA_SAMPLE_CONNECT_DONE:
        PRINT("%s::DHCP start.\r\n", WIFI_STA_SAMPLE_LOG);
        netif_p = netifapi_netif_find(ifname);
        if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
            PRINT("%s::find netif or start DHCP fail, try again !\r\n", WIFI_STA_SAMPLE_LOG);
            wifi_state_event_cb(WIFI_STA_SAMPLE_DISCONNECT, NULL);
        }
        osThreadAttr_t attr;
        attr.name       = "example_check_dhcp_status";
        attr.attr_bits  = 0U;
        attr.cb_mem     = NULL;
        attr.cb_size    = 0U;
        attr.stack_mem  = NULL;
        attr.stack_size = WIFI_TASK_STACK_SIZE;
        attr.priority   = WIFI_TASK_PRIO;
        if (osThreadNew((osThreadFunc_t)example_check_dhcp_status, NULL, &attr) == NULL) {
            PRINT("%s::Create example_check_dhcp_status fail.\r\n", WIFI_STA_SAMPLE_LOG);
        } else {
            PRINT("%s::Create example_check_dhcp_status succ.\r\n", WIFI_STA_SAMPLE_LOG);
        }
        break;
    case WIFI_STA_SAMPLE_DISCONNECT:
        wifi_sta_scan();
        break;
    case WIFI_STA_SAMPLE_GOT_IP:
        PRINT("%s::STA GOT IP success.\r\n", WIFI_STA_SAMPLE_LOG);
        PRINT("%s::IP ADDR:%s\r\n", WIFI_STA_SAMPLE_LOG, ip4addr_ntoa(netif_ip4_addr(netif_p)));
        PRINT("%s::NETMASK:%s\r\n", WIFI_STA_SAMPLE_LOG, ip4addr_ntoa(netif_ip4_netmask(netif_p)));
        PRINT("%s::GATEWAY:%s\r\n", WIFI_STA_SAMPLE_LOG, ip4addr_ntoa(netif_ip4_gw(netif_p)));
        break;
    default:
        PRINT("%s::Invalid event id:%d.\r\n", WIFI_STA_SAMPLE_LOG, event);
        break;
    }
}

int sta_sample_init(void)
{
    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        PRINT("%s::wifi_event_cb register fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    PRINT("%s::wifi_event_cb register succ.\r\n", WIFI_STA_SAMPLE_LOG);
    /* 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 1: 等待100ms后判断状态 */
    }
    PRINT("%s::wifi init succ.\r\n", WIFI_STA_SAMPLE_LOG);

    if (wifi_sta_enable() != 0) {
        PRINT("%s::wifi_sta_enable fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }
    if (wifi_sta_set_reconnect_policy(0, 2, 1, 1) != ERRCODE_SUCC) {
        PRINT("%s::wifi_sta_set_reconnect_policy fail.\r\n", WIFI_STA_SAMPLE_LOG);
        return -1;
    }

    return 0;
}

