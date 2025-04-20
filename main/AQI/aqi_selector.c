
#include "aqi_selector.h"

#include "ui.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_SELECTOR_FORMAT "%s_%s"

#define AQI_SELECTOR_CONTAINER_SIZE   736, 256
#define AQI_SELECTOR_CONTAINER_OFFSET 0, 68

#define AQI_SELECTOR_ITEM_SIZE      730, 48
#define AQI_SELECTOR_ITEM_OFFSET(i) 0, i * 48

#define AQI_SELECTOR_ICON_OFFSET 0, 0
#define AQI_SELECTOR_NAME_OFFSET 32, 0
#define AQI_SELECTOR_TICK_OFFSET 0, 0

static const char *TAG = "AQI SELECTOR";

aqi_selector_t *aqi_selector_create(uint8_t index, const char *item_name, lv_obj_t *container, const lv_img_dsc_t *flag,
                                    void (*event_handler)(lv_event_t *event))
{
    lv_obj_t *item = aqi_setting_create_item(container, AQI_SELECTOR_ITEM_SIZE, AQI_SELECTOR_ITEM_OFFSET(index));
    lv_obj_t *icon = aqi_setting_create_image(item, flag, LV_ALIGN_LEFT_MID, AQI_SELECTOR_ICON_OFFSET);
    lv_obj_t *name = aqi_setting_create_label(item, item_name, LV_ALIGN_LEFT_MID, AQI_SELECTOR_NAME_OFFSET);
    lv_obj_t *tick = aqi_setting_create_image(item, &ui_img_images_tick_png, LV_ALIGN_RIGHT_MID, AQI_SELECTOR_TICK_OFFSET);
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
        lv_obj_set_style_bg_color(selector->item, AQI_SETTING_ITEM_BG_COLOR_SELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(selector->tick, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_set_style_bg_color(selector->item, AQI_SETTING_ITEM_BG_COLOR_UNSELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_flag(selector->tick, LV_OBJ_FLAG_HIDDEN);
    }
}

void aqi_selector_update_language(aqi_selector_t *selector, char *type, char *locale)
{
    lv_label_set_text(selector->name, _(aqi_selector_label_name_locale(type, locale)));
}
