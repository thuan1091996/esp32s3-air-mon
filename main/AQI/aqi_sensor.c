
#include "aqi_sensor.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_err.h"
#include "esp_log.h"

#include "apc1.h"
#include "aqi_config.h"
#include "aqi_indicator_ui.h"

#define AQI_SENSOR_TASK_NAME       "AQI Sensor Task"
#define AQI_SENSOR_TASK_STACK_SIZE (5 * 1024)
#define AQI_SENSOR_TASK_PRIORITY   (5)

typedef struct {
    aqi_config_sensor_t *config;
} aqi_sensor_t;

static const char *TAG = "AQI SENSOR";

static aqi_sensor_t _aqi_sensor = {0};

extern bool example_lvgl_lock(int timeout_ms);
extern void example_lvgl_unlock(void);

static void __aqi_sensor_measurement_task(void *pvParameters)
{
    while (true)
    {
        if (apc1_measurement() == ESP_OK)
        {
            example_lvgl_lock(-1);
            aqi_indicator_ui_data_show( apc1_get_pm1_0(),
                                        apc1_get_pm2_5(),
                                        apc1_get_pm10(),
                                        apc1_get_ECO2(),
                                        apc1_get_T_comp(),
                                        apc1_get_RH_comp(),
                                        apc1_get_TVOC(),
                                        apc1_get_AQI());
            example_lvgl_unlock();
        }
        else
        {
            ESP_LOGE(TAG, "%s", _aqi_sensor.config->error_message);
        }

        vTaskDelay(pdMS_TO_TICKS(_aqi_sensor.config->period_ms));
    }
}

void aqi_sensor_init()
{
    _aqi_sensor.config = aqi_config_sensor_get();

    acp1_init();
    apc1_read_infor();

    xTaskCreate(&__aqi_sensor_measurement_task, AQI_SENSOR_TASK_NAME,
                AQI_SENSOR_TASK_STACK_SIZE, NULL, AQI_SENSOR_TASK_PRIORITY, NULL);
}
