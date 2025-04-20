
#ifndef _AQI_SETTING_H
#define _AQI_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "lvgl.h"

#define AQI_SETTING_ITEM_BG_COLOR_SELECT   lv_color_hex(0x30323B)
#define AQI_SETTING_ITEM_BG_COLOR_UNSELECT lv_color_hex(0x1F2127)

int aqi_setting_init();

lv_obj_t *aqi_setting_create_container(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs);
lv_obj_t *aqi_setting_create_item(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs);
lv_obj_t *aqi_setting_create_image(lv_obj_t *parent, const void *src, lv_align_t align, int x_offset, int y_offset);
lv_obj_t *aqi_setting_create_label(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
