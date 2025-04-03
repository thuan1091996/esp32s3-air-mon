
#include "aqi_language.h"

#include "ui.h"

#include "esp_err.h"
#include "esp_log.h"

typedef struct {
} aqi_language_t;

static const char *TAG = "AQI LANGUAGE";

static aqi_language_t _aqi_language = {0};

int aqi_language_init()
{
    return ESP_OK;
}

int aqi_language_config_factory()
{
    lv_img_set_src(ui_ImageSettingStageLanguage, &ui_img_images_stage_green_png);
    return ESP_OK;
}
