# hi3863_demo
## 支持的开发板

|                          开发板名称                          | 硬件资料                                                     | 软件资料     | 购买链接                                                     | 开发板介绍                                                   |
| :----------------------------------------------------------: | ------------------------------------------------------------ | :----------- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| HiHope_NearLink_DK3863E_V03<img src="docs/pic/tools/image-20240809164437807.png" alt="image-20240809164437807" style="zoom: 50%;" /> | [扩展板硬件资料](https://gitee.com/HiSpark/fbb_ws63/tree/master/docs/hardware/HiHope_NearLink_DK_WS63E_V03) | 参考示例教程 | [开发板购买链接](https://main.m.taobao.com/security-h5-detail/home?id=808369265492&spm=a2141.7631565.tbshopmod-guess_your_like.808369265492&scm=1007.18373.317678.0&pvid=fdb854bd-99a3-4b65-955b-392bdf0c083e&fromNormal=true) | 基于海思WS63E解决方案的一款高度集成的2.4Ghz SoC Wi-Fi、BLE、SLE的开发板，支持Liteos、OpenHarmony轻量系统 |
| BearPi-Pico_H3863<img src="docs/pic/BearPi-Pico_H3863/bearpi_pico_h3863.png" alt="image-20240809164437807" style="zoom: 50%;" /> | [扩展板硬件资料](https://gitee.com/HiSpark/fbb_ws63/tree/master/docs/hardware/BearPi-Pico_H3863) | 参考示例教程 | [开发板购买链接](https://item.taobao.com/item.htm?id=821386760379) | BearPi-Pico H3863是一款基于高度集成2.4GHz Wi-Fi6 、BLE、SLE为主控芯片的核心板，具有灵活的数字接口，集成高性能 32bit 微处理器（MCU），硬件安全引擎以及丰富的外设接口，外设接口包括 SPI、UART、I2C、PWM、GPIO，支持 6 路 13bit 分辨率 ADC，内置 SRAM 和合封 Flash，并支持在 Flash 上运行程序支持Liteos、OpenHarmony轻量系统 |
## 示例介绍
| 名称 | 描述 |
| ----------- | ----------- |
| mqtt_client_demo | 通过WIFI连接上路由器，并与自己搭建的EMQX服务器建立MQTT TCP连接 |
| http_server_demo | 通过WIFI连接上路由器，绑定IP和端口，通过浏览器访问HTTP SERVER |
| http_client_demo | 通过WIFI连接上路由器，通过HTTP GET百度网页数据|
