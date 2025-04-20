
#ifndef _AQI_SELECTOR_H
#define _AQI_SELECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "lvgl.h"

#define AQI_SELECTOR_MAX 9

typedef struct {
    uint8_t index;
    lv_obj_t* item;
    lv_obj_t* icon;
    lv_obj_t* name;
    lv_obj_t* tick;
} aqi_selector_t;

aqi_selector_t *aqi_selector_create(uint8_t index, const char *item_name, lv_obj_t *container, const lv_img_dsc_t *flag,
                                    void (*event_handler)(lv_event_t *event));

char *aqi_selector_label_name_locale(char *type, char *locale);

void aqi_selector_update_item(aqi_selector_t *selector, uint8_t select);
void aqi_selector_update_language(aqi_selector_t *selector, char *type, char *locale);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
