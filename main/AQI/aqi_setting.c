
#include "aqi_setting.h"

#include "aqi_factory.h"

#include "aqi_language.h"
#include "aqi_country.h"
#include "aqi_wifi.h"

#include "ui.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_SETTING_CONTAINER_STATUS_SIZE 800, 47

#define AQI_SETTING_CONTAINER_BG_COLOR AQI_SETTING_ITEM_BG_COLOR_SELECT
#define AQI_SETTING_CONTAINER_BG_OPA   82
#define AQI_SETTING_CONTAINER_PADDING  12
#define AQI_SETTING_CONTAINER_RADIUS   8

#define AQI_SETTING_ITEM_BG_OPA         255
#define AQI_SETTING_ITEM_PADDING_LEFT   20
#define AQI_SETTING_ITEM_PADDING_RIGHT  12
#define AQI_SETTING_ITEM_PADDING_TOP    10
#define AQI_SETTING_ITEM_PADDING_BOTTOM 10

typedef struct {
    uint8_t had_factory_setting;
} aqi_setting_t;

static const char *TAG = "AQI SETTING";

static aqi_setting_t _aqi_setting = {0};

static lv_style_t _aqi_setting_container_style;
static lv_style_t _aqi_setting_item_style;
static lv_style_t _aqi_setting_name_style;

static void __aqi_setting_init_styles()
{
    // Style for container
    lv_style_init(&_aqi_setting_container_style);
    lv_style_set_bg_color(&_aqi_setting_container_style, AQI_SETTING_CONTAINER_BG_COLOR);
    lv_style_set_bg_opa(&_aqi_setting_container_style, AQI_SETTING_CONTAINER_BG_OPA);
    lv_style_set_pad_all(&_aqi_setting_container_style, AQI_SETTING_CONTAINER_PADDING);
    lv_style_set_radius(&_aqi_setting_container_style, AQI_SETTING_CONTAINER_RADIUS);

    // Style for item
    lv_style_init(&_aqi_setting_item_style);
    lv_style_set_bg_color(&_aqi_setting_item_style, AQI_SETTING_ITEM_BG_COLOR_UNSELECT);
    lv_style_set_bg_opa(&_aqi_setting_item_style, AQI_SETTING_ITEM_BG_OPA);
    lv_style_set_pad_left(&_aqi_setting_item_style, AQI_SETTING_ITEM_PADDING_LEFT);
    lv_style_set_pad_right(&_aqi_setting_item_style, AQI_SETTING_ITEM_PADDING_RIGHT);
    lv_style_set_pad_top(&_aqi_setting_item_style, AQI_SETTING_ITEM_PADDING_TOP);
    lv_style_set_pad_bottom(&_aqi_setting_item_style, AQI_SETTING_ITEM_PADDING_BOTTOM);

    // Style for name
    lv_style_init(&_aqi_setting_name_style);
    lv_style_set_text_font(&_aqi_setting_name_style, &ui_font_Poppins_Regular_18);
}

static int __aqi_setting_components_init()
{
    static uint8_t initialized = 0;

    if (initialized == 0)
    {
        initialized = 1;
        aqi_wifi_init();
        aqi_country_init();        
        aqi_language_init();
    }

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

            // Update the status container
            lv_obj_set_parent(ui_ContainerStatus, ui_ContainerSettingMain);
            lv_obj_set_size(ui_ContainerStatus, AQI_SETTING_CONTAINER_STATUS_SIZE);

            __aqi_setting_components_init();

            if (_aqi_setting.had_factory_setting == 0)
            {
                aqi_factory_init();
            }
        }
    }
}

int aqi_setting_init()
{
    __aqi_setting_init_styles();

    lv_obj_add_event_cb(ui_ScreenSettingAQI, &__aqi_setting_event_handler, LV_EVENT_SCREEN_LOADED, NULL);

    return ESP_OK;
}

lv_obj_t *aqi_setting_create_container(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, &_aqi_setting_container_style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_align(container, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    return container;
}

lv_obj_t *aqi_setting_create_item(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_add_style(item, &_aqi_setting_item_style, 0);
    lv_obj_set_size(item, w, h);
    lv_obj_align(item, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    return item;
}

lv_obj_t *aqi_setting_create_image(lv_obj_t *parent, const void *src, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *img = lv_img_create(parent);
    lv_img_set_src(img, src);
    lv_obj_align_to(img, parent, align, x_offset, y_offset);
    return img;
}

lv_obj_t *aqi_setting_create_label(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_align_to(label, parent, align, x_offset, y_offset);
    lv_obj_add_style(label, &_aqi_setting_name_style, 0);
    return label;
}
