
#include "aqi_country.h"

#include "ui.h"

#include "esp_err.h"
#include "esp_log.h"

typedef struct {
} aqi_country_t;

static const char *TAG = "AQI COUNTRY";

static aqi_country_t _aqi_country = {0};

int aqi_country_init()
{
    return ESP_OK;
}

int aqi_country_config_factory()
{
    lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_green_png);
    return ESP_OK;
}
