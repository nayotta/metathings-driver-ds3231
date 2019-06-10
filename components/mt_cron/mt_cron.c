#include "esp_log.h"

#include "cron.h"

#include "mt_nvs_storage.h"
#include "mt_sntp.h"
#include "mt_cron.h"

static const char *TAG = "MT_CRON";

extern MT_SNTP_SYNED;

typedef struct {
  int len;
  uint8_t *buf;

} mt_cron_task_t;

static esp_err_t mt_cron_set_tasks(int len, uint8_t *time_tasks) {
  esp_err_t ret;

  if (mt_nvs_write_byte_config("time_tasks", len, time_tasks) == false) {
    ESP_LOGE(TAG, "%d mt_nvs_write_byte_config failed", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

static esp_err_t mt_cron_get_tasks(int *len_out, uint8_t *time_tasks_out) {
  time_tasks_out = mt_nvs_read_byte_config("time_tasks", len_out);

  if (time_tasks_out == NULL) {
    ESP_LOGW(TAG, "%d no time tasks saved", __LINE__);
    return ESP_ERR_INVALID_STATE;
  }

  return ESP_OK;
}

static void mt_cron_callback(struct cron_job_struct *data) {
  ESP_LOGI(TAG, "cron task, id=%d", data->id);

  mt_cron_task_t *task = malloc(sizeof(mt_cron_task_t));  // free in func
  task = (mt_cron_task_t *)data->data;
#if 0
    // debug info
    int i;
    for (i = 0; i < task->len; i++)
    {
        printf("%.2x ", task->buf[i]);
    }
    printf("\n");
#endif
  nyt_message_process(task->buf, task->len);
}

static void nyt_cron_create_jobs() {
  uint8_t *time_tasks_byte = NULL;
  int time_tasks_len = 0;
  SetTimeTaskReq *time_tasks;

  time_tasks_byte = nyt_cron_get_tasks(&time_tasks_len);
  if (time_tasks_byte == NULL) {
    ESP_LOGE(TAG, "no time tasks saved");
  } else {
    time_tasks = set_time_task_req__unpack(NULL, time_tasks_len,
                                           time_tasks_byte);  // free in func
    for (int i = 0; i < time_tasks->n_time_tasks; i++) {
      for (int j = 0; j < time_tasks->time_tasks[i]->n_tasks; j++) {
        ESP_LOGI(TAG, "cron time:%s, task:%d", time_tasks->time_tasks[i]->time,
                 time_tasks->time_tasks[i]->tasks[j]->payload_case);

        int task_len = mqtt_device_request__get_packed_size(
            time_tasks->time_tasks[i]->tasks[j]);
        uint8_t *task_buf = malloc(task_len);
        mqtt_device_request__pack(time_tasks->time_tasks[i]->tasks[j],
                                  task_buf);
        mt_cron_task_t *task = malloc(sizeof(mt_cron_task_t));
        task->len = task_len;
        task->buf = task_buf;
#if 0
                // debug info
                int k;
                uint8_t *buf = task_buf;
                for (k = 0; k < task_len; k++)
                {
                    printf("%.2x ", buf[k]);
                }
                printf("\n");
#endif
        if (cron_job_create(time_tasks->time_tasks[i]->time, mt_cron_callback,
                            task) == NULL) {
          ESP_LOGE(TAG, "failed cron_job_create");
        }
      }
    }
    set_time_task_req__free_unpacked(time_tasks, NULL);
  }
}

static void nyt_cron_loop() {
  while (MT_SNTP_SYNED != true) {
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
  }
  ESP_LOGI(TAG, "cron create job");

  nyt_cron_create_jobs();
  cron_start();
  while (1) {
    vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
#if 0
        time_t next;
        next = cron_job_seconds_until_next_execution();
        ESP_LOGI(TAG, "cron heartbeat, next=%ld", next);
#endif
  }

  vTaskDelete(NULL);
}

void nyt_cron_task_restart() {
  cron_job_clear_all();
  cron_stop();
  nyt_cron_create_jobs();
  cron_start();
}

void nyt_cron_task() {
  xTaskCreate((TaskFunction_t)nyt_cron_loop, "nyt_cron_task", 8192, NULL, 8,
              NULL);
}
