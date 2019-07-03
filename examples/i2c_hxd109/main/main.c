#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stdio.h"

#include "hxd019_i2c.h"

void app_main()
{
  mt_i2c_hxd019_i2c_init();

      while (1)
  {
    mt_i2c_hxd019_i2c_learn();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}
