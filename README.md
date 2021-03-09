# metathings-esp32-lib
    metathings esp32 开发库


## 模块

| 模块                              | 描述                                                                 | 授权 | 线程安全|
|-----------------------------------|---------------------------------------------------------------------|------|--------|
| **cryptolib**                     | cryptoauthlib 基础库                                                 | GNU   | 是    |
| **huawei_mqtt**                   | 华为 roma 内网平台                                                   | -      | 是    |
| **i2cdev**                        | ESP32 i2c driver                                                     | GNU  | -     |
| **mt_ethernet**                   | metathings 有线网络任务                                               | -    | -     |
| **mt_http_client**                | metathings http client                                               | -    | -     |
| **mt_json_parse**                 | metathings json file parse(unfinish)                                 | -    | -     |
| **mt_memory_manage**              | metathings 内存管理                                                   | -    | -     |
| **mt_modbus_master**              | metathings modbus master                                             | -    | -     |
| **mt_modbus_master_airswitch001** | metathings modbus master for airswitch only                          | -    | -     |
| **mt_module_flow**                | metathings module flow 相关函数                                       | -    | -     |
| **mt_module_http**                | metathings module http client api                                    | -    | -     |
| **mt_module_http_manage**         | metathings module http manage 多module管理                           | -    | -     |
| **mt_module_lora**                | metathings module lora 串口接收数据处理                               | -    | -     |
| **mt_module_mqtt**                | metathings module mqtt mqtt接收数据处理                               | -    | -     |
| **mt_module_unarycall**           | metathings module unarycall 工具函数                                 | -    | -     |
| **mt_module_unarycall_config**    | metathings module unarycall 系统配置函数                              | -    | -     |
| **mt_module_unarycall_monitor**   | metathings module unarycall 系统监测函数                              | -    | -     |
| **mt_module_unarycall_ota**       | metathings module unarycall 系统ota                                  | -    | -     |
| **mt_mqtt**                       | metathings mqtt mqtt task 循环                                       | -    | -     |
| **mt_mqtt_lan**                   | metathings mqtt 网络mqtt                                             | -    | -     |
| **mt_mqtt_manage**                | metathings mqtt 多mqtt客户端管理                                      | -    | -     |
| **mt_network**                    | metathings network  ethernet or wifi                                 | -    | -     |
| **mt_nvs_config**                 | metathings nvs config 配置存储                                        | -    | -     |
| **mt_nvs_storage**                | metathings nvs storage 键值对持久存储                                 | -    | 是    |
| **mt_ota**                        | metathings ota task                                                  | -    | -     |
| **mt_protobuf_c**                 | metathings protobuf-c 库                                             | -    | -     |
| **mt_rtc_time**                   | metathings 硬件rtc时间 读写                                           | -    | 是    |
| **mt_smartconfig**                | metathings smartconfig 和 airkiss配网                                | -    | 是    |
| **mt_sntp**                       | metathings wifi时间 硬件rtc时间 系统时间三者同步服务                    | -    | 是    |
| **mt_sys_config**                 | metathings 系统配置函数                                               | -    | -     |
| **mt_sys_monitor**                | metathings 系统监测函数                                               | -    | -     |
| **mt_utils**                      | metathings 工具函数                                                   | -    | -     |
| **rs232_dev**                     | metathings rs232基础库                                                | -    | -     |


## 应用
      
| 模块                              | 描述                                                                     | 授权 | 线程安全|
|-----------------------------------|-------------------------------------------------------------------------|------|--------|
| **gpio_button**                   | 硬件gpio 按键,短按--长按                                                 | -     | 是     |
| **gpio_light**                    | 硬件gpio 灯, 亮灭,自定义时间闪烁                                          | -     | 是     |
| **gpio_maglock001**               | 硬件gpio 门磁,常通常闭由设备决定                                          | -     | 是     |
| **gpio_pulse_motor**              | 硬件gpio 脉冲电机驱动                                                    | -     | 是     |
| **i2c_atecc608a**                 | 硬件I2C 加密芯片,安全密钥存储器                                           | MIT  | -      |
| **i2c_ds1307**                    | 硬件I2C 时钟模块                                                         | MIT  | 是     |
| **i2c_ds3231**                    | 硬件I2C 时钟模块附带温度测量                                              | MIT  | 是     |
| **modbus_aew100**                 | 硬件RS485 电流互感器(acrel aew100)                                       | -    | 是     |
| **modbus_airswitch001**           | 硬件RS485 智能空气开关(曼顿空气开关)                                      | -    | 是     |
| **modbus_bits001**                | 硬件RS485 烟雾温湿度传感器(毕达斯物联)                                    | -    | 是     |
| **modbus_gzpd800T**               | 硬件RS485 局放传感器(公众智联 gzpd800T)                                  | -    | 是     |
| **modbus_ktc_n01**                | 硬件RS485 空调控制器(建大仁科 ktc-n01)                                   | -    | 是     |
| **modbus_ld100**                  | 硬件RS485 水浸传感器(立科ld100))                                         | -    | 是     |
| **modbus_power001**               | 硬件RS485 电表(德力西))                                                  | -    | 是     |
| **modbus_reclose001**             | 硬件RS485 自动重合闸(莱源))                                               | -    | 是     |
| **modbus_xycwa6a**                | 硬件RS485 无线温度传感器6路(xycwa6a)                                     | -    | 是     |
| **rs232_EA900II**                 | 硬件RS232 不间断电源(EA900II)                                            | -    | 是     |
| **rs232_PRS3342**                 | 硬件RS232 中压配电柜(prs3342)                                            | -    | 是     |
| **rs232_charge001**               | 硬件RS232 充电桩(德力西10路)                                             | -    | 是     |
| **rs232_lora_ebyte**              | 硬件RS232 lora模块(ebyte E22)                                           | -    | 是     |
| **rs232_lora_jxyl001**            | 硬件RS232 lora烟雾温湿度传感器(捷讯易联)                                  | -    | 是     |
| **rs232_sim_air720h**             | 硬件RS232 4G模组(air724 air720HH))                                        | -    | 是     |


## 参考

    esp32官方 [ESP-IDF framework](https://github.com/espressif/esp-idf)

    参考来源 [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)

    参考来源 [esp-idf-lib](https://github.com/UncleRus/esp-idf-lib)
