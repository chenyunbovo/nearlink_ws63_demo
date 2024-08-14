#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/netifapi.h"
#include "lwip/api.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "http_server.h"

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG         LWIP_DBG_OFF
#endif

#define PORT 80

static const char http_html_hdr[] =
    "HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n\r\n";
static const char http_index_html[] =
"<html><head><title>Congrats!</title></head>\
<body><h1 align=\"center\">Hello World!</h1>\
<h2 align=\"center\">Welcome to Fire lwIP HTTP Server!</h1>\
<p align=\"center\">This is a small test page, served by httpserver-netconn.</p>\
<p align=\"center\"><a href=\"http://www.baidu.com/\">访问百度</a>\
</body></html>";

static void  http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    /* 读取数据 */
    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void**)&buf, &buflen);

        /* 判断是不是HTTP的GET命令*/
        if (buflen>=5 &&
                buf[0]=='G' &&
                buf[1]=='E' &&
                buf[2]=='T' &&
                buf[3]==' ' &&
                buf[4]=='/' )
        {

            /* 发送数据头 */
            netconn_write(conn, http_html_hdr,
            sizeof(http_html_hdr)-1, NETCONN_NOCOPY);
            /* 发送网页数据 */
            netconn_write(conn, http_index_html,
                sizeof(http_index_html)-1, NETCONN_NOCOPY);
        }
    }
    netconn_close(conn); /* 关闭连接 */

    /* 释放inbuf */
    netbuf_delete(inbuf);
}

int http_server_netconn_thread(void *param)
{
    td_char ifname[17] = "wlan0"; /* 创建的STA接口名 */
    struct netif *netif_p = TD_NULL;
    netif_p = netifapi_netif_find(ifname);

    UNUSED(param);
    err_t err;
    struct netconn *conn, *newconn;
    const ip_addr_t server_ip = netif_p->ip_addr;
    printf("http server bind ip: %s;PORT:%d\r\n", ip4addr_ntoa(netif_ip4_addr(netif_p)), PORT);
    /* 创建netconn连接结构 */
    /* 绑定端口号与IP地址，端口号默认是80 */
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, &server_ip, PORT);

    LWIP_ERROR("http_server: invalid conn", (conn != NULL), return -1;);

    /* 监听 */
    netconn_listen(conn);

    do
    {
        //处理连接请求
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            //发送网页数据
            http_server_netconn_serve(newconn);

            //删除连接结构
            netconn_delete(newconn);
        }
    }
    while (err == ERR_OK);
    //关闭
    netconn_close(conn);
    netconn_delete(conn);
    return 0;
}

void  http_server_netconn_init(void)
{
    osThreadAttr_t attr;
    attr.name       = "http_server_netconn";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = 2048;
    attr.priority   = 12;
    if (osThreadNew((osThreadFunc_t)http_server_netconn_thread, NULL, &attr) == NULL) {
        PRINT("Create http_server_netconn_thread fail.\r\n");
    }
}
