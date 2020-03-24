# 说明

通讯参数 2400 821

485引脚
tx 13
rx 15
en 05

接口

esp_err_t modbus_power001_get_quality(UCHAR port, double *data);
esp_err_t modbus_power001_get_voltage(UCHAR port, double *data);
esp_err_t modbus_power001_get_current(UCHAR port, double *data);
modbus_power001_data_t *modbus_power001_get_data(int port);
modbus_power001_datas_t *modbus_power001_get_datas();
