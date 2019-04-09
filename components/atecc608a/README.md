# metathings-atecc608a
<https://github.com/MicrochipTech/cryptoauthlib/issues/5> and  
<https://github.com/MicrochipTech/cryptoauthlib/issues/28>

## 接口

atecc608A

| 接口     | 描述       |         | 接口     |描述            |
|----------|-----------|---------|----------|---------------|
| **NC**   |           |<------->| **VCC**  | 2.0v-5.5V     | 
| **NC**   |           |<------->| **NC**   |               |
| **NC**   |           |<------->| **SCL**  | SCL           | 
| **GND**  |   GND     |<------->| **SDA**  | SDA           | 

esp32

| 接口     | esp32接口  |
|----------|-----------|
| **SCL**  | IO17/TX2  |
| **SDA**  | IO16/RX2  |

接口配置在componens/cryptoauthlib/cryptoauthlib/lib/hal/hal_esp32_i2c.c 不能从外部更改