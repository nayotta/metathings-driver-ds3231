# 说明

通讯参数 9600 8n1

485引脚
tx 23
rx 22
en 18

接口
esp_err_t mt_ld100_get_addr(int addr, bool *state);

esp_err_t mt_ld100_set_addr(int addr, int new_addr);

esp_err_t mt_ld100_get_state(int addr, bool *state);

esp_err_t mt_ld100_get_relay(int addr, bool *state);

esp_err_t mt_ld100_set_relay(int addr, bool state);

# 版本
##  v1.0
huawei
