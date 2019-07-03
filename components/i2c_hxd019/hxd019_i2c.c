#include "unistd.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"

#include "hxd019_i2c.h"

// global value ===============================================================
static const char *TAG = "MT_I2C_HXD019_I2C";

#define MT_I2C_HXD019_SDA 18
#define MT_I2C_HXD019_SCL 19
#define MT_I2C_HXD019_BUSY 5

// global func ================================================================
esp_err_t mt_i2c_hxd019_i2c_init()
{
    int i2c_master_port = I2C_NUM_0;
    i2c_config_t conf;
    esp_err_t err = ESP_OK;

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = MT_I2C_HXD019_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = MT_I2C_HXD019_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d i2c_param_config failed", __LINE__);
        return err;
    }

    err = i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d i2c_driver_install failed", __LINE__);
        return err;
    }

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << MT_I2C_HXD019_BUSY);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    //io_conf.pull_up_en = 0;
    err = gpio_config(&io_conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d gpio_config failed", __LINE__);
        return err;
    }

    return ESP_OK;
}

esp_err_t mt_i2c_hxd019_i2c_learn()
{
    esp_err_t err = ESP_OK;
    uint8_t addr = 0x30;
    uint8_t *data = malloc(230);

    /*
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_write_byte(cmd, 0xff, false);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1 * 1000 / portTICK_RATE_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d i2c_master_cmd_begin failed code=%d", __LINE__, err);
        return err;
    }*/
    err = gpio_set_level(MT_I2C_HXD019_SCL, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d gpio_set_level failed code=%d", __LINE__, err);
        return err;
    }
    usleep(50);
    err = gpio_set_level(MT_I2C_HXD019_SCL, 1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d gpio_set_level failed code=%d", __LINE__, err);
        return err;
    }

    vTaskDelay(20 / portTICK_RATE_MS);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    //i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x30, true);
    i2c_master_write_byte(cmd, 0x20, true);
    i2c_master_write_byte(cmd, 0x50, true);
    //i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 20 * 1000 / portTICK_RATE_MS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "%d i2c_master_cmd_begin failed code=%d", __LINE__, err);
        return err;
    }
    i2c_cmd_link_delete(cmd);

    while (gpio_get_level(MT_I2C_HXD019_BUSY) == 0)
    {
        usleep(10);
    }
    usleep(50);
    cmd = i2c_cmd_link_create();
    for (int i = 0; i < 230; i++)
    {
        i2c_master_read_byte(cmd, data[i], true);
    }
    i2c_master_stop(cmd);

    for (int i = 0; i < 230; i++)
    {
        printf("%2x ", data[i]);
    }
    printf("\n");
    printf("\n");

    return ESP_OK;
}
