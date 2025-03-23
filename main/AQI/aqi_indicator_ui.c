
#include "aqi_indicator_ui.h"
#include "aqi_indicator_ui_define.h"

#include "ui.h"

#include "esp_log.h"


// AQI UI Controller
typedef struct {
    uint8_t indicator_num;
    aqi_indicator_t *indicator[AQI_INDICATOR_NUM];
    aqi_indicator_t *selection;
} aqi_controller_t;

static aqi_indicator_attribute_t _attribute_list[] = {
    ATTRIBUTE_INIT(PM1),
    ATTRIBUTE_INIT(PM25),
    ATTRIBUTE_INIT(PM10),
    ATTRIBUTE_INIT(CO2),
    ATTRIBUTE_INIT(TEMP),
    ATTRIBUTE_INIT(RH),
    ATTRIBUTE_INIT(TVOC)
};

static aqi_controller_t _aqi_controller = {0};

static void __update_value(aqi_indicator_t *self, uint16_t new_value)
{
    if (self->attribute->data.value != new_value)
    {
        self->attribute->data.value = new_value;

        self->update_display(self);

        if (self == _aqi_controller.selection)
        {
            self->update_select(self);
        }
    }
}

static void __update_display(aqi_indicator_t *self)
{
    // Update value label
    lv_label_set_text_fmt(*self->attribute->ui.value_label, "%d", self->attribute->data.value);

    // Align components
    lv_obj_align(*self->attribute->ui.value_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(*self->attribute->ui.unit_label, *self->attribute->ui.value_label, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_align(*self->attribute->ui.container, LV_ALIGN_TOP_MID, 0, 0);

    // Update bar color based on thresholds
    lv_color_t color = (self->attribute->data.value < self->attribute->data.threshold_1) ? lv_color_hex(COLOR_GREEN) :
                       (self->attribute->data.value < self->attribute->data.threshold_2) ? lv_color_hex(COLOR_ORANGE) :
                                                                                           lv_color_hex(COLOR_RED);
    lv_obj_set_style_bg_color(*self->attribute->ui.bar, color, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_update_layout(*self->attribute->ui.container);
}

static void __update_select(aqi_indicator_t *self)
{
    lv_label_set_text_fmt(ui_LabelValueSelectAQI, "%d", self->attribute->data.value);

    const lv_img_dsc_t *image_src = (self->attribute->data.value < self->attribute->data.threshold_1) ? self->attribute->ui.image_green :
                                    (self->attribute->data.value < self->attribute->data.threshold_2) ? self->attribute->ui.image_orange :
                                                                                                        self->attribute->ui.image_red;
    lv_img_set_src(ui_ImageLevelSelectAQI, image_src);
}

static void __select(aqi_indicator_t *self, uint8_t selected)
{
    if (self->attribute->ui.line_top)
    {
        lv_obj_set_style_bg_opa(*self->attribute->ui.line_top, selected ? LINE_OPA_RESET : LINE_OPA_SET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (self->attribute->ui.line_bot)
    {
        lv_obj_set_style_bg_opa(*self->attribute->ui.line_bot, selected ? LINE_OPA_RESET : LINE_OPA_SET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_bg_opa(*self->attribute->ui.click_container_cover, selected ? COVER_OPA_SET : COVER_OPA_RESET, LV_PART_MAIN | LV_STATE_DEFAULT);

    if (selected)
    {
        lv_label_set_text(ui_LabelNameSelectAQI, self->attribute->data.name);
        lv_label_set_text(ui_LabelUnitSelectAQI, self->attribute->data.unit);
        self->update_select(self);
    }
}

static void __aqi_indicator_ui_event_handler(lv_event_t *event)
{
    if (lv_event_get_code(event) == LV_EVENT_CLICKED)
    {
        aqi_indicator_t *new_selection = (aqi_indicator_t*)lv_event_get_user_data(event);

        ESP_LOGI("AQI", "Click event on %s", new_selection->attribute->data.name);

        if (new_selection && new_selection != _aqi_controller.selection)
        {
            _aqi_controller.selection->select(_aqi_controller.selection, 0);
            _aqi_controller.selection = new_selection;
            _aqi_controller.selection->select(_aqi_controller.selection, 1);
        }
    }
}

// Constructor for aqi_indicator_t
aqi_indicator_t* create_aqi_indicator(aqi_indicator_attribute_t *attribute)
{
    aqi_indicator_t *indicator = malloc(sizeof(aqi_indicator_t));

    indicator->attribute = attribute;

    // Assign methods
    indicator->update_value = __update_value;
    indicator->update_display = __update_display;
    indicator->update_select = __update_select;
    indicator->select = __select;

    return indicator;
}

void aqi_indicator_ui_init()
{
    uint8_t attribute_num = sizeof(_attribute_list) / sizeof(_attribute_list[0]);

    if (attribute_num > AQI_INDICATOR_NUM)
    {
        ESP_LOGE("AQI", "Attribute number is greater than the indicator number");
        return;
    }

    _aqi_controller.indicator_num = attribute_num;

    // Initialize indicator
    for (int i = 0; i < attribute_num; i++)
    {
        _aqi_controller.indicator[i] = create_aqi_indicator(&_attribute_list[i]);
        lv_obj_add_event_cb(*_aqi_controller.indicator[i]->attribute->ui.click_container_cover,
                            __aqi_indicator_ui_event_handler, LV_EVENT_ALL, _aqi_controller.indicator[i]);
        lv_obj_add_event_cb(*_aqi_controller.indicator[i]->attribute->ui.click_panel,
                            __aqi_indicator_ui_event_handler, LV_EVENT_ALL, _aqi_controller.indicator[i]);
    }

    _aqi_controller.selection = _aqi_controller.indicator[0];
    _aqi_controller.selection->select(_aqi_controller.selection, 1);
}

void aqi_indicator_ui_data_show(uint16_t pm1, uint16_t pm25, uint16_t pm10, uint16_t co2, uint16_t temp, uint16_t humi, uint16_t tvoc)
{
    uint16_t values[_aqi_controller.indicator_num];

    values[0] = pm1;
    values[1] = pm25;
    values[2] = pm10;
    values[3] = co2;
    values[4] = temp;
    values[5] = humi;
    values[6] = tvoc;

    for (int i = 0; i < _aqi_controller.indicator_num; i++)
    {
        _aqi_controller.indicator[i]->update_value(_aqi_controller.indicator[i], values[i]);
    }
}
