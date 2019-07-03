#include "driver/gpio.h"
#include "unistd.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "hxd019.h"

// global value ===============================================================
static const char *TAG = "MT_I2C_HXD019";

#define MT_I2C_HXD019_SDA 18
#define MT_I2C_HXD019_SCL 19
#define MT_I2C_HXD019_BUSY 5

uint32_t MT_I2C_HXD019_SCL_INTERVAL = 30; // 30us
uint32_t MT_I2C_HXD019_SCL_MIN_ST = 20;   // SCL 第一个下降沿到二的时间 20mS

// static func =================================================================
// sda func
static void mt_i2c_hxd019_set_sda_io()
{
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.pin_bit_mask = (1ULL << MT_I2C_HXD019_SDA);
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf.pull_down_en = 0;
  //io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}

static void mt_i2c_hxd019_set_sda_input()
{
  gpio_pad_select_gpio(MT_I2C_HXD019_SDA);
  gpio_set_direction(MT_I2C_HXD019_SDA, GPIO_MODE_INPUT);
}

static void mt_i2c_hxd019_set_sda_output()
{
  gpio_pad_select_gpio(MT_I2C_HXD019_SDA);
  gpio_set_direction(MT_I2C_HXD019_SDA, GPIO_MODE_OUTPUT);
}

static void mt_i2c_hxd019_set_sda_high()
{
  gpio_set_level(MT_I2C_HXD019_SDA, 1);
}

static void mt_i2c_hxd019_set_sda_low()
{
  gpio_set_level(MT_I2C_HXD019_SDA, 0);
}

static int8_t mt_i2c_hxd019_get_sda()
{
  return gpio_get_level(MT_I2C_HXD019_SDA);
}

// scl func
static void mt_i2c_hxd019_set_scl_io()
{
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.pin_bit_mask = (1ULL << MT_I2C_HXD019_SCL);
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf.pull_down_en = 0;
  //io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}

static void mt_i2c_hxd019_set_scl_input()
{
  gpio_set_direction(MT_I2C_HXD019_SCL, GPIO_MODE_INPUT);
}

static void mt_i2c_hxd019_set_scl_output()
{
  gpio_set_direction(MT_I2C_HXD019_SCL, GPIO_MODE_OUTPUT);
}

static void mt_i2c_hxd019_set_scl_high()
{
  gpio_set_level(MT_I2C_HXD019_SCL, 1);
}

static void mt_i2c_hxd019_set_scl_low()
{
  gpio_set_level(MT_I2C_HXD019_SCL, 0);
}

static int8_t mt_i2c_hxd019_get_scl()
{
  return gpio_get_level(MT_I2C_HXD019_SCL);
}

// busy func
static void mt_i2c_hxd019_set_busy_io()
{
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.pin_bit_mask = (1ULL << MT_I2C_HXD019_BUSY);
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
  io_conf.pull_down_en = 0;
  //io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}

static void mt_i2c_hxd019_set_busy_input()
{
  gpio_set_direction(MT_I2C_HXD019_BUSY, GPIO_MODE_INPUT);
}

static void mt_i2c_hxd019_set_busy_output()
{
  gpio_set_direction(MT_I2C_HXD019_BUSY, GPIO_MODE_OUTPUT);
}

static void mt_i2c_hxd019_set_busy_high()
{
  gpio_set_level(MT_I2C_HXD019_BUSY, 1);
}

static void mt_i2c_hxd019_set_busy_low()
{
  gpio_set_level(MT_I2C_HXD019_BUSY, 0);
}

static int8_t mt_i2c_hxd019_get_busy()
{
  return gpio_get_level(MT_I2C_HXD019_BUSY);
}

static void mt_i2c_hxd019_delay(uint32_t time)
{
  usleep(time);
  return;
}

static void mt_i2c_hxd019_remote_delay(uint32_t time)
{
  time = time * 1000;
  usleep(time);
}

static uint8_t mt_i2c_hxd019_get_ack()
{
  uint8_t ack = 0;

  mt_i2c_hxd019_set_sda_input();
  mt_i2c_hxd019_delay(4 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_delay(4 * MT_I2C_HXD019_SCL_INTERVAL);

  ack = mt_i2c_hxd019_get_sda();
  mt_i2c_hxd019_delay(4 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(4 * MT_I2C_HXD019_SCL_INTERVAL);

  return ack;
}

static void mt_i2c_hxd019_send_ack()
{
  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_delay(MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_sda_low();
  mt_i2c_hxd019_delay(MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_delay(MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
}

static void mt_i2c_hxd019_open()
{
  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_set_scl_output();
  mt_i2c_hxd019_set_sda_high();
  mt_i2c_hxd019_set_scl_high();
}

static void mt_i2c_hxd019_close()
{
  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_set_scl_output();
  mt_i2c_hxd019_set_sda_high();
  mt_i2c_hxd019_set_scl_high();
}

static void mt_i2c_hxd019_start()
{
  mt_i2c_hxd019_open();
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_sda_low();
  mt_i2c_hxd019_delay(10 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);
}

static void mt_i2c_hxd019_stop()
{
  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_set_scl_output();
  mt_i2c_hxd019_set_sda_low();
  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_sda_high();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
}

static void mt_i2c_hxd019_write_byte(uint8_t bData)
{
  uint8_t data_bit, ack;
  int i = 0;

  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_set_scl_output();
  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  for (i = 7; i >= 0; i--)
  {
    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

    data_bit = (bData >> i) & 0x01;

    if (data_bit)
      mt_i2c_hxd019_set_sda_high();
    else
      mt_i2c_hxd019_set_sda_low();

    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
    mt_i2c_hxd019_set_scl_high();
    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
    mt_i2c_hxd019_set_scl_low();
  }

  ack = mt_i2c_hxd019_get_ack();

  return ack;
}

static void mt_i2c_hxd019_write(uint8_t *data, uint8_t count)
{
  uint8_t byte = 0;

  mt_i2c_hxd019_open();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_start();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  for (int i = 0; i < count; i++)
  {
    byte = data[i];
    mt_i2c_hxd019_write_byte(byte);
    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
  }

  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_stop();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_close();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
}

static void mt_i2c_hxd019_read_byte(uint8_t *pData)
{
  uint8_t ack = 0;
  uint8_t readdata = 0;

  mt_i2c_hxd019_set_sda_input();
  for (int i = 8; i >= 0; i--)
  {
    readdata <<= 1;
    mt_i2c_hxd019_set_scl_high();
    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

    readdata |= mt_i2c_hxd019_get_sda();

    mt_i2c_hxd019_set_scl_low();

    *pData = readdata;

    mt_i2c_hxd019_send_ack();
    mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);
  }
}

static void mt_i2c_hxd019_read(uint8_t *data)
{
  uint8_t bValue;
  uint8_t checksum;

  mt_i2c_hxd019_open();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_remote_delay(MT_I2C_HXD019_SCL_MIN_ST);

  mt_i2c_hxd019_start();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_write_byte(0x30);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_write_byte(0x62);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_start();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_write_byte(0x31);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_read_byte(&bValue);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  data[0] = bValue;
  checksum = 0xc3;
  for (int i = 1; i < 230; i++)
  {
    mt_i2c_hxd019_read_byte(&bValue);
    mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

    data[i] = bValue;
    checksum += bValue;
  }

  mt_i2c_hxd019_read_byte(&bValue);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_stop();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_close();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
}

// global func ================================================================
void mt_i2c_hxd019_init()
{
  mt_i2c_hxd019_set_sda_io();
  mt_i2c_hxd019_set_sda_output();
  mt_i2c_hxd019_set_sda_high();

  mt_i2c_hxd019_set_scl_io();
  mt_i2c_hxd019_set_scl_output();
  mt_i2c_hxd019_set_scl_high();

  mt_i2c_hxd019_set_busy_io();
  mt_i2c_hxd019_set_busy_input();
}

void mt_i2c_hxd019_learn_start()
{
  mt_i2c_hxd019_open();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_low();
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_set_scl_high();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_write_byte(0x30);
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_write_byte(0x20);

  mt_i2c_hxd019_write_byte(0x50);
  mt_i2c_hxd019_delay(2 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_stop();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);

  mt_i2c_hxd019_close();
  mt_i2c_hxd019_delay(1 * MT_I2C_HXD019_SCL_INTERVAL);
}

void mt_i2c_hxd019_learn()
{
  uint8_t *data = malloc(230);
  uint32_t count = 20 * 100 * 1000;

  mt_i2c_hxd019_learn_start();
  vTaskDelay(50 / portTICK_PERIOD_MS);
  while (mt_i2c_hxd019_get_busy() == 1 && count != 0)
  {
    usleep(10);
    count--;
  }

  if (count == 0)
  {
    ESP_LOGE(TAG, "%d %s timeout", __LINE__, __func__);
    return;
  }

  mt_i2c_hxd019_read(data);

  for (int i = 0; i < 230; i++)
  {
    printf("%2x ", data[i]);
  }
  printf("\n");
  printf("\n");
}
