
#ifndef _AQI_INDICATOR_UI_H
#define _AQI_INDICATOR_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "aqi_config.h"

#include "lvgl.h"

typedef struct aqi_indicator_t aqi_indicator_t;

typedef struct {
    lv_obj_t **value_label;
    lv_obj_t **unit_label;
    lv_obj_t **bar;
    lv_obj_t **line_top;
    lv_obj_t **line_bot;
    lv_obj_t **click_container_cover;
    lv_obj_t **click_panel;

    // Images for different indicator level
    const lv_img_dsc_t *image_good;
    const lv_img_dsc_t *image_warning;
    const lv_img_dsc_t *image_bad;
} aqi_indicator_ui_t;

typedef struct {
    uint16_t value;                 // Current value
    aqi_config_indicator_t *config; // Configuration data
    aqi_indicator_ui_t ui;          // UI Components
} aqi_indicator_attribute_t;

struct aqi_indicator_t {
    aqi_indicator_attribute_t *attribute;

    // Methods (function pointers)
    void (*update_value)(aqi_indicator_t *self, uint16_t new_value);
    void (*update_display)(aqi_indicator_t *self);
    void (*update_select)(aqi_indicator_t *self);
    
    void (*select)(aqi_indicator_t *self, uint8_t selected);    // selected != :0 Select otherwise deselect the indicator
};

void aqi_indicator_ui_init();

void aqi_indicator_ui_data_show(uint16_t pm1, uint16_t pm2_5, uint16_t pm10, uint16_t co2,
                                uint16_t temp, uint16_t humi, uint16_t tvoc, uint16_t aqi);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
