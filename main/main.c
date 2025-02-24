#include "esp_err.h"
#include "esp_log.h"

#include "apc1_define.h"
#include "apc1.h"
#include "waveshare_rgb_lcd_port.h"

#define TAG                     "main"
#define APC1_SENSOR_TEST        (1)

void app_main()
{
    if (acp1_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "acp1_init fail");
        return;
    }
    else
    {
        ESP_LOGI(TAG, "ACP1 information read");
    }
#if (APC1_SENSOR_TEST != 0)
    while(1)
    {
        if(apc1_measurement() != ESP_OK)
        {
            ESP_LOGE(TAG, "Error reading ACP1 measurement");
        }
        else
        {
            ESP_LOGI(TAG, "ACP1 measurement read");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
#endif /* End of (APC1_SENSOR_TEST != 0) */
    waveshare_esp32_s3_rgb_lcd_init(); // Initialize the Waveshare ESP32-S3 RGB LCD 
    // wavesahre_rgb_lcd_bl_on();  //Turn on the screen backlight 
    // wavesahre_rgb_lcd_bl_off(); //Turn off the screen backlight 
    
    ESP_LOGI(TAG, "Display LVGL demos");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(-1)) {
        // lv_demo_stress();
        // lv_demo_benchmark();
        // lv_demo_music();
        lv_demo_widgets();
        // example_lvgl_demo_ui();
        // Release the mutex
        lvgl_port_unlock();
    }
}
