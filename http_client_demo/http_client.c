#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "http_client.h"

int8_t get_buf[] = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\n\r\n";

#define HOST_NAME "www.baidu.com" 
#define PORT 80

int http_client_thread(void *param)
{
    UNUSED(param);
    int sock = -1,rece;
    struct sockaddr_in client_addr;

    uint8_t *pbufdata = (uint8_t *)malloc(12000*sizeof(uint8_t));
    if(pbufdata == NULL) {
        PRINT("malloc failed\n");
        return -1;
    }
    char* host_ip;

    ip_addr_t dns_ip;
    u32_t dns_ip_len = 1;
    netconn_gethostbyname(HOST_NAME, &dns_ip, &dns_ip_len);
    host_ip = ip_ntoa(&dns_ip);
    PRINT_DEBUG("host name : %s , host_ip : %s\n",HOST_NAME,host_ip);
    for (;;) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Socket error\n");
            (void)osDelay(10);
            continue;
        }
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(PORT);
        client_addr.sin_addr.s_addr = inet_addr(host_ip);
        memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));
        if (connect(sock,(struct sockaddr *)&client_addr,sizeof(struct sockaddr)) == -1) {
            printf("Connect failed!\n");
            closesocket(sock);
            (void)osDelay(10);
            continue;
        }
        printf("Connect to server successful!\n");
        printf("\n***************************************************\n");
        send(sock,get_buf,sizeof(get_buf),0);
        while(1) {
            rece = recv(sock, pbufdata, 12000, 0);
            if (rece <= 0) {
                break;
            }
            printf("rece:%d\r\n",rece);
        }
        printf("\n***************************************************\n");
        closesocket(sock);
        (void)osDelay(100000);
    }
    return 0;
}

void  http_client_init(void)
{
    osThreadAttr_t attr;
    attr.name       = "http_client";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = 4096;
    attr.priority   = 12;
    if (osThreadNew((osThreadFunc_t)http_client_thread, NULL, &attr) == NULL) {
        PRINT("Create http_client_thread fail.\r\n");
    }
}
