
#include "aqi_sensor.h"

#include "aqi_indicator_ui.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdlib.h>

#include "esp_err.h"
#include "esp_log.h"

#include "apc1.h"

#define AQI_SENSOR_MEASUREMENT_PERIOD   1000

static const char *TAG = "AQI SENSOR";

static void __aqi_sensor_measurement_task(void *pvParameters)
{
    while (1)
    {
        if (apc1_measurement() == ESP_OK)
        {
            aqi_indicator_ui_data_show( apc1_get_pm1_0(),
                                        apc1_get_pm2_5(),
                                        apc1_get_pm10(),
                                        apc1_get_ECO2(),
                                        apc1_get_T_comp(),
                                        apc1_get_RH_comp(),
                                        apc1_get_TVOC());
        }
        else
        {
            ESP_LOGE(TAG, "Measurement ACP1 fail");
        }

        vTaskDelay(AQI_SENSOR_MEASUREMENT_PERIOD / portTICK_PERIOD_MS);
    }
}

void aqi_sensor_init()
{
    acp1_init();
    apc1_read_infor();

    // Create the measurement task
    xTaskCreate(&__aqi_sensor_measurement_task, "aqi_sensor_measurement_task", 10 * 1024, NULL, 5, NULL);
}
