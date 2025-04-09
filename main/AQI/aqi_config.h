
#ifndef _AQI_CONFIG_H
#define _AQI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define AQI_CONFIG_INDICATOR_NUM 7

#define AQI_CONFIG_SENSOR_ERROR_MESSAGE_SIZE 50

#define AQI_CONFIG_INDICATOR_NAME_SIZE 20
#define AQI_CONFIG_INDICATOR_UNIT_SIZE 20

typedef struct {
    uint8_t color_primary;
    uint8_t color_secondary;
    bool dark_mode;
} aqi_config_theme_t;

typedef struct {
    uint32_t period_ms;
    char error_message[AQI_CONFIG_SENSOR_ERROR_MESSAGE_SIZE];
} aqi_config_sensor_t;

typedef struct {
    uint16_t threshold_warning;
    uint16_t threshold_bad;
    uint32_t color_good;
    uint32_t color_warning;
    uint32_t color_bad;
    char name[AQI_CONFIG_INDICATOR_NAME_SIZE];
    char unit[AQI_CONFIG_INDICATOR_UNIT_SIZE];
} aqi_config_indicator_t;

int aqi_config_init();

aqi_config_theme_t* aqi_config_theme_get();
aqi_config_sensor_t* aqi_config_sensor_get();
aqi_config_indicator_t* aqi_config_indicator_get(uint8_t index);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
