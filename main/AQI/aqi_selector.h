
#ifndef _AQI_SELECTOR_H
#define _AQI_SELECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "lvgl.h"

#define AQI_SELECTOR_MAX 9

#define AQI_SELECTOR_ITEM_BG_COLOR_SELECT   lv_color_hex(0x30323B)
#define AQI_SELECTOR_ITEM_BG_COLOR_UNSELECT lv_color_hex(0x1F2127)

typedef struct {
    uint8_t index;
    lv_obj_t* item;
    lv_obj_t* icon;
    lv_obj_t* name;
    lv_obj_t* tick;
} aqi_selector_t;

int aqi_selector_init();

aqi_selector_t *aqi_selector_create(uint8_t index, const char *item_name, lv_obj_t *container, const lv_img_dsc_t *flag,
                                    void (*event_handler)(lv_event_t *event));

lv_obj_t *aqi_selector_create_container(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs);
lv_obj_t *aqi_selector_create_item(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs);
lv_obj_t *aqi_selector_create_image(lv_obj_t *parent, const void *src, lv_align_t align, int x_offset, int y_offset);
lv_obj_t *aqi_selector_create_label(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset);
                                    
char *aqi_selector_label_name_locale(char *type, char *locale);

void aqi_selector_update_item(aqi_selector_t *selector, uint8_t select);
void aqi_selector_update_language(aqi_selector_t *selector, char *type, char *locale);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
