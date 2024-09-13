#include "lwip/sntp.h"
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

#define DIFF_SEC_1970_2036          ((u32_t)2085978496L)

const uint8_t mounth_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t STNP_GET_TIME_FLAG = 0;

struct tm *ptm;
int time_clock_display(void *param)
{
    UNUSED(param);
    uint32_t time = osKernelGetTickCount();
    while(1) {
        if (STNP_GET_TIME_FLAG == 0) {
            time += 1;
        } else {
            printf("sntp time: %d-%02d-%02d %02d:%02d:%02d\r\n", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
            ptm->tm_sec++;
            if (ptm->tm_sec >= 60) {
                ptm->tm_sec = 0;
                ptm->tm_min++;
                if (ptm->tm_min >= 60) {
                    ptm->tm_min = 0;
                    ptm->tm_hour++;
                    if (ptm->tm_hour >= 24) {
                        ptm->tm_hour = 0;
                        ptm->tm_mday++;
                        if (ptm->tm_mday > mounth_day[ptm->tm_mon - 1]) {
                            ptm->tm_mday = 1;
                            ptm->tm_mon++;
                            if (ptm->tm_mon > 12) {
                                ptm->tm_mon = 1;
                                ptm->tm_year++;
                            }
                        }
                    }
                }
            }
            time += 100;
        }
        osDelayUntil(time);
    }
    return 0;
}

void sntp_client_init(void)
{
    PRINT("sntp_client_init\r\n");
    osThreadAttr_t attr;
    attr.name       = "time_clock_display";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = 2048;
    attr.priority   = 17;
    if (osThreadNew((osThreadFunc_t)time_clock_display, NULL, &attr) == NULL) {
        PRINT("Create time_clock_display fail.\r\n");
    } else {
        PRINT("Create time_clock_display succ.\r\n");
    }
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    char *sntp_server[1] = {"pool.ntp.org"};
    struct timeval time_local;
    int res = lwip_sntp_start(1, sntp_server, &time_local);
    if (res != 0) {
        PRINT("lwip_sntp_start fail, res = %d\r\n", res);
    } else {
        PRINT("lwip_sntp_start success\r\n");
        printf("sntp time: %ld, %ld\r\n", time_local.tv_sec, time_local.tv_usec);
        time_local.tv_sec = time_local.tv_sec + DIFF_SEC_1970_2036 + 8 * 3600;
        ptm = localtime(&(time_local.tv_sec));
        ptm->tm_year += 1900;
        ptm->tm_mon += 1;
        STNP_GET_TIME_FLAG = 1;
    }
}
