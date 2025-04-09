
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
    ATTRIBUTE_DEFAULT(PM1, pm),
    ATTRIBUTE_DEFAULT(PM25, pm),
    ATTRIBUTE_DEFAULT(PM10, pm),
    ATTRIBUTE_DEFAULT(CO2, co2),
    ATTRIBUTE_DEFAULT(TEMP, temp),
    ATTRIBUTE_DEFAULT(RH, rh),
    ATTRIBUTE_DEFAULT(TVOC, tvoc),
};

static const char *TAG = "AQI INDICATOR";

static aqi_controller_t _aqi_controller = {0};

static void __update_value(aqi_indicator_t *self, uint16_t new_value)
{
    if (self->attribute->value != new_value)
    {
        self->attribute->value = new_value;

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
    lv_label_set_text_fmt(*self->attribute->ui.value_label, "%d", self->attribute->value);

    // Align components
    lv_obj_align(*self->attribute->ui.value_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(*self->attribute->ui.unit_label, *self->attribute->ui.value_label,
                    LV_ALIGN_OUT_RIGHT_MID, AQI_INDICATOR_UNIT_ALIGN_TO_LABEL_OFFSET, 0);

    // Update bar color based on thresholds
    lv_color_t color;
    if (self->attribute->value < self->attribute->config->threshold_warning)
        color = lv_color_hex(self->attribute->config->color_good);
    else if (self->attribute->value < self->attribute->config->threshold_bad)
        color = lv_color_hex(self->attribute->config->color_warning);
    else
        color = lv_color_hex(self->attribute->config->color_bad);
    lv_obj_set_style_bg_color(*self->attribute->ui.bar, color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

static void __update_select(aqi_indicator_t *self)
{
    lv_label_set_text_fmt(ui_LabelValueSelectAQI, "%d", self->attribute->value);

    const lv_img_dsc_t *image_src;
    if (self->attribute->value < self->attribute->config->threshold_warning)
        image_src = self->attribute->ui.image_good;
    else if (self->attribute->value < self->attribute->config->threshold_bad)
        image_src= self->attribute->ui.image_warning;
    else
        image_src = self->attribute->ui.image_bad;
    lv_img_set_src(ui_ImageLevelSelectAQI, image_src);
}

static void __select(aqi_indicator_t *self, uint8_t selected)
{
    if (self->attribute->ui.line_top)
    {
        lv_obj_set_style_bg_opa(*self->attribute->ui.line_top,
                                selected ? AQI_INDICATOR_LINE_OPA_RESET : AQI_INDICATOR_LINE_OPA_SET,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (self->attribute->ui.line_bot)
    {
        lv_obj_set_style_bg_opa(*self->attribute->ui.line_bot,
                                selected ? AQI_INDICATOR_LINE_OPA_RESET : AQI_INDICATOR_LINE_OPA_SET,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_bg_opa(*self->attribute->ui.click_container_cover,
                            selected ? AQI_INDICATOR_COVER_OPA_SET : AQI_INDICATOR_COVER_OPA_RESET,
                            LV_PART_MAIN | LV_STATE_DEFAULT);

    if (selected)
    {
        lv_label_set_text(ui_LabelNameSelectAQI, self->attribute->config->name);
        lv_label_set_text(ui_LabelUnitSelectAQI, self->attribute->config->unit);
        self->update_select(self);
    }
}

static void __aqi_indicator_ui_event_handler(lv_event_t *event)
{
    if (lv_event_get_code(event) == LV_EVENT_CLICKED)
    {
        aqi_indicator_t *new_selection = (aqi_indicator_t*)lv_event_get_user_data(event);

        ESP_LOGI(TAG, "Click event on %s", new_selection->attribute->config->name);

        if (new_selection && new_selection != _aqi_controller.selection)
        {
            _aqi_controller.selection->select(_aqi_controller.selection, 0);
            _aqi_controller.selection = new_selection;
            _aqi_controller.selection->select(_aqi_controller.selection, 1);
        }
    }
    else if (lv_event_get_target(event) == ui_ScreenIndicatorAQI)
    {
        if (lv_event_get_code(event) == LV_EVENT_SCREEN_LOADED)
        {
            ESP_LOGI(TAG, "Indicator screen loaded event triggered");
            lv_obj_set_parent(ui_ContainerStatus, ui_ContainerIndicator);
            lv_obj_set_size(ui_ContainerStatus, AQI_INDICATOR_CONTAINER_STATUS_SIZE);
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
        ESP_LOGE(TAG, "Attribute number is greater than the indicator number");
        return;
    }

    _aqi_controller.indicator_num = attribute_num;

    // Initialize indicator
    for (int i = 0; i < attribute_num; i++)
    {
        _attribute_list[i].config = aqi_config_indicator_get(i);
        _aqi_controller.indicator[i] = create_aqi_indicator(&_attribute_list[i]);
        _aqi_controller.indicator[i]->update_display(_aqi_controller.indicator[i]);
        lv_obj_add_event_cb(*_aqi_controller.indicator[i]->attribute->ui.click_container_cover,
                            __aqi_indicator_ui_event_handler, LV_EVENT_CLICKED, _aqi_controller.indicator[i]);
        lv_obj_add_event_cb(*_aqi_controller.indicator[i]->attribute->ui.click_panel,
                            __aqi_indicator_ui_event_handler, LV_EVENT_CLICKED, _aqi_controller.indicator[i]);
    }

    _aqi_controller.selection = _aqi_controller.indicator[0];
    _aqi_controller.selection->select(_aqi_controller.selection, 1);

    lv_obj_add_event_cb(ui_ScreenIndicatorAQI, &__aqi_indicator_ui_event_handler, LV_EVENT_SCREEN_LOADED, NULL);
}

void aqi_indicator_ui_data_show(uint16_t pm1, uint16_t pm25, uint16_t pm10, uint16_t
                                co2, uint16_t temp, uint16_t humi, uint16_t tvoc)
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
