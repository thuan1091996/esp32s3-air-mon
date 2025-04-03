
#include "aqi_utility.h"

#include "ui.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_UTILITY_TASK_NAME         "AQI Utility Task"
#define AQI_UTILITY_TASK_STACK_SIZE   (5 * 1024)
#define AQI_UTILITY_TASK_PRIORITY     5

static const char *TAG = "AQI UTILITY";

struct tm __aqi_utility_get_current_time()
{
    struct tm timeinfo = { 0 };
    time_t now;
    
    time(&now);                    // Get the current time
    localtime_r(&now, &timeinfo);  // Convert to local time

    return timeinfo;
}

static void __aqi_utility_task_handler(void *pvParameters)
{
    struct tm timeinfo = { 0 };

    while (true)
    {
        timeinfo = __aqi_utility_get_current_time();

        if (timeinfo.tm_sec == 0)   // Update time every minute
        {
            ESP_LOGI(TAG, "New minute started:%s", asctime(&timeinfo));
            aqi_utility_update_time(timeinfo);
        }
    
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int aqi_utility_init()
{
    aqi_utility_update_time(__aqi_utility_get_current_time());

    xTaskCreate(__aqi_utility_task_handler, AQI_UTILITY_TASK_NAME, AQI_UTILITY_TASK_STACK_SIZE,
                NULL, AQI_UTILITY_TASK_PRIORITY, NULL);
    
    return ESP_OK;
}

int aqi_utility_update_time(struct tm timeinfo)
{
    lv_label_set_text_fmt(ui_LabelTime, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return ESP_OK;
}
