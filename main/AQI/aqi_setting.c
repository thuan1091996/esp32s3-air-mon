
#include "aqi_setting.h"

#include "aqi_language.h"
#include "aqi_country.h"
#include "aqi_wifi.h"

#include "ui.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

typedef enum {
    aqi_setting_stage_none,
    aqi_setting_stage_language,
    aqi_setting_stage_country,
    aqi_setting_stage_wifi,
    aqi_setting_stage_done
} 
aqi_setting_stage_t;

typedef struct {
    uint8_t initialized;
    aqi_setting_stage_t stage;
} aqi_setting_t;

static const char *TAG = "AQI SETTING";

static aqi_setting_t _aqi_setting = {0};

static void __aqi_setting_event_handler(lv_event_t *event);

static int __aqi_setting_components_init()
{
    if (_aqi_setting.initialized == 0)
    {
        _aqi_setting.initialized = 1;

        aqi_language_init();
        aqi_country_init();
        aqi_wifi_init();
    }

    return ESP_OK;
}

static int __aqi_setting_config_factory()
{
    lv_obj_clear_flag(ui_ContainerSettingTitle, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingButton, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);

    aqi_language_config_factory();
    aqi_country_config_factory();

    _aqi_setting.stage = aqi_setting_stage_wifi;    // bypass language and country stage (update later)
    lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
    aqi_wifi_config_factory();

    return ESP_OK;
}

static void __aqi_setting_event_handler(lv_event_t *event)
{
    lv_obj_t *target = lv_event_get_target(event);

    if (target == ui_ScreenSettingAQI)
    {
        if (lv_event_get_code(event) == LV_EVENT_SCREEN_LOADED)
        {
            ESP_LOGI(TAG, "Setting screen loaded event triggered");
            lv_obj_set_parent(ui_ContainerStatus, ui_ContainerSettingMain);
            lv_obj_set_size(ui_ContainerStatus, 800, 47);
            __aqi_setting_components_init();
            __aqi_setting_config_factory();
        }
    }
    else if (target == ui_ButtonSettingBack)
    {
        ESP_LOGI(TAG, "Back button clicked");
    }
    else if (target == ui_ButtonSettingNext)
    {
        ESP_LOGI(TAG, "Next button clicked");
    }
}

int aqi_setting_init()
{
    lv_obj_add_event_cb(ui_ScreenSettingAQI, &__aqi_setting_event_handler, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(ui_ButtonSettingBack, &__aqi_setting_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_ButtonSettingNext, &__aqi_setting_event_handler, LV_EVENT_CLICKED, NULL);

    return ESP_OK;
}
