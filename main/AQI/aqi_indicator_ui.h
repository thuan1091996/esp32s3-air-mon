
#ifndef _AQI_INDICATOR_UI_H
#define _AQI_INDICATOR_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "lvgl.h"

void aqi_indicator_ui_init();

void aqi_indicator_ui_data_show(uint16_t pm1, uint16_t pm2_5, uint16_t pm10, uint16_t co2,
                                uint16_t temp, uint16_t humi, uint16_t tvoc);

void aqi_indicator_ui_click_event_handler(lv_obj_t *target);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
