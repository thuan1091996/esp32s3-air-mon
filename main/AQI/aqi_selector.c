
#include "aqi_selector.h"

#include "ui.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_SELECTOR_FORMAT "%s_%s"

#define AQI_SELECTOR_CONTAINER_BG_COLOR AQI_SELECTOR_ITEM_BG_COLOR_SELECT
#define AQI_SELECTOR_CONTAINER_BG_OPA   82
#define AQI_SELECTOR_CONTAINER_PADDING  12
#define AQI_SELECTOR_CONTAINER_RADIUS   8

#define AQI_SELECTOR_ITEM_BG_OPA         255
#define AQI_SELECTOR_ITEM_PADDING_LEFT   20
#define AQI_SELECTOR_ITEM_PADDING_RIGHT  12
#define AQI_SELECTOR_ITEM_PADDING_TOP    10
#define AQI_SELECTOR_ITEM_PADDING_BOTTOM 10

#define AQI_SELECTOR_CONTAINER_SIZE   736, 256
#define AQI_SELECTOR_CONTAINER_OFFSET 0, 68

#define AQI_SELECTOR_ITEM_SIZE      730, 48
#define AQI_SELECTOR_ITEM_OFFSET(i) 0, i * 48

#define AQI_SELECTOR_ICON_OFFSET 0, 0
#define AQI_SELECTOR_NAME_OFFSET 32, 0
#define AQI_SELECTOR_TICK_OFFSET 0, 0

static const char *TAG = "AQI SELECTOR";

static lv_style_t _aqi_selector_container_style;
static lv_style_t _aqi_selector_item_style;
static lv_style_t _aqi_selector_name_style;

static void __aqi_selector_init_styles()
{
    // Style for container
    lv_style_init(&_aqi_selector_container_style);
    lv_style_set_bg_color(&_aqi_selector_container_style, AQI_SELECTOR_CONTAINER_BG_COLOR);
    lv_style_set_bg_opa(&_aqi_selector_container_style, AQI_SELECTOR_CONTAINER_BG_OPA);
    lv_style_set_pad_all(&_aqi_selector_container_style, AQI_SELECTOR_CONTAINER_PADDING);
    lv_style_set_radius(&_aqi_selector_container_style, AQI_SELECTOR_CONTAINER_RADIUS);

    // Style for item
    lv_style_init(&_aqi_selector_item_style);
    lv_style_set_bg_color(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_BG_COLOR_UNSELECT);
    lv_style_set_bg_opa(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_BG_OPA);
    lv_style_set_pad_left(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_PADDING_LEFT);
    lv_style_set_pad_right(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_PADDING_RIGHT);
    lv_style_set_pad_top(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_PADDING_TOP);
    lv_style_set_pad_bottom(&_aqi_selector_item_style, AQI_SELECTOR_ITEM_PADDING_BOTTOM);

    // Style for name
    lv_style_init(&_aqi_selector_name_style);
    lv_style_set_text_font(&_aqi_selector_name_style, &ui_font_Poppins_Regular_18);
}

int aqi_selector_init()
{
    __aqi_selector_init_styles();
    return ESP_OK;
}

aqi_selector_t *aqi_selector_create(uint8_t index, const char *item_name, lv_obj_t *container, const lv_img_dsc_t *flag,
                                    void (*event_handler)(lv_event_t *event))
{
    lv_obj_t *item = aqi_selector_create_item(container, AQI_SELECTOR_ITEM_SIZE, AQI_SELECTOR_ITEM_OFFSET(index));
    lv_obj_t *icon = aqi_selector_create_image(item, flag, LV_ALIGN_LEFT_MID, AQI_SELECTOR_ICON_OFFSET);
    lv_obj_t *name = aqi_selector_create_label(item, item_name, LV_ALIGN_LEFT_MID, AQI_SELECTOR_NAME_OFFSET);
    lv_obj_t *tick = aqi_selector_create_image(item, &ui_img_images_tick_png, LV_ALIGN_RIGHT_MID, AQI_SELECTOR_TICK_OFFSET);
    lv_obj_add_flag(tick, LV_OBJ_FLAG_HIDDEN);

    aqi_selector_t *selector = malloc(sizeof(aqi_selector_t));
    if (selector == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for selector");
        return NULL;
    }

    selector->index = index;
    selector->item = item;
    selector->icon = icon;
    selector->name = name;
    selector->tick = tick;
    lv_obj_add_event_cb(item, event_handler, LV_EVENT_CLICKED, selector);

    return selector;
}

lv_obj_t *aqi_selector_create_container(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, &_aqi_selector_container_style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_align(container, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    return container;
}

lv_obj_t *aqi_selector_create_item(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_add_style(item, &_aqi_selector_item_style, 0);
    lv_obj_set_size(item, w, h);
    lv_obj_align(item, LV_ALIGN_TOP_MID, x_ofs, y_ofs);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
    return item;
}

lv_obj_t *aqi_selector_create_image(lv_obj_t *parent, const void *src, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *img = lv_img_create(parent);
    lv_img_set_src(img, src);
    lv_obj_align_to(img, parent, align, x_offset, y_offset);
    return img;
}

lv_obj_t *aqi_selector_create_label(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_align_to(label, parent, align, x_offset, y_offset);
    lv_obj_add_style(label, &_aqi_selector_name_style, 0);
    return label;
}

char *aqi_selector_label_name_locale(char *type, char *locale)
{
    static char label_name[sizeof(AQI_SELECTOR_FORMAT) + 10];
    snprintf(label_name, sizeof(label_name), AQI_SELECTOR_FORMAT, type, locale);
    return label_name;
}

void aqi_selector_update_item(aqi_selector_t *selector, uint8_t select)
{
    if (select)
    {
        lv_obj_set_style_bg_color(selector->item, AQI_SELECTOR_ITEM_BG_COLOR_SELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(selector->tick, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_set_style_bg_color(selector->item, AQI_SELECTOR_ITEM_BG_COLOR_UNSELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(selector->tick, LV_OBJ_FLAG_HIDDEN);
    }
}

void aqi_selector_update_language(aqi_selector_t *selector, char *type, char *locale)
{
    lv_label_set_text(selector->name, _(aqi_selector_label_name_locale(type, locale)));
}
