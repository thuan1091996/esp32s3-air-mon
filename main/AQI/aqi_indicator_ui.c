
#include "aqi_indicator_ui.h"

#include "ui.h"

#include "freertos/FreeRTOS.h"

#define COVER_OPA_SET   82
#define COVER_OPA_RESET 0
#define LINE_OPA_SET    255
#define LINE_OPA_RESET  0

/* 
 * Color to show the quality level of the bar object
 */
#define COLOR_GREEN     0x20A979
#define COLOR_ORANGE    0xF79009
#define COLOR_RED       0xFF2D46

/* 
 * TODO: Define the appropriate level thresholds
 * Min and max values ​​as in the data sheet of ACP1 sensor
 */
#define PM1_VALUE_MIN           0
#define PM1_LEVEL_THRESHOLD_1   300
#define PM1_LEVEL_THRESHOLD_2   400
#define PM1_VALUE_MAX           500

#define PM2_5_VALUE_MIN         0
#define PM2_5_LEVEL_THRESHOLD_1 500
#define PM2_5_LEVEL_THRESHOLD_2 800
#define PM2_5_VALUE_MAX         1000

#define PM10_VALUE_MIN          0
#define PM10_LEVEL_THRESHOLD_1  800
#define PM10_LEVEL_THRESHOLD_2  1300
#define PM10_VALUE_MAX          1500

#define CO2_VALUE_MIN           400
#define CO2_LEVEL_THRESHOLD_1   30000
#define CO2_LEVEL_THRESHOLD_2   50000
#define CO2_VALUE_MAX           65000

#define TEMP_VALUE_MIN          0
#define TEMP_LEVEL_THRESHOLD_1  25
#define TEMP_LEVEL_THRESHOLD_2  40
#define TEMP_VALUE_MAX          50

#define RH_VALUE_MIN            0
#define RH_LEVEL_THRESHOLD_1    50
#define RH_LEVEL_THRESHOLD_2    80
#define RH_VALUE_MAX            100

#define TVOC_VALUE_MIN          0
#define TVOC_LEVEL_THRESHOLD_1  30000
#define TVOC_LEVEL_THRESHOLD_2  50000
#define TVOC_VALUE_MAX          65000

#define SET_UNIT_DATA(data, _threshold_1, _threshold_2, _name, _unit)\
                            data.threshold_1 = _threshold_1;\
                            data.threshold_2 = _threshold_2;\
                            data.name = _name;\
                            data.unit = _unit;

typedef enum {
    AQI_INDICATOR_UI_PM1,
    AQI_INDICATOR_UI_PM2_5,
    AQI_INDICATOR_UI_PM10,
    AQI_INDICATOR_UI_CO2,
    AQI_INDICATOR_UI_TEMP,
    AQI_INDICATOR_UI_RH,
    AQI_INDICATOR_UI_TVOC
} aqi_indicator_ui_select_t;

typedef struct {
    lv_obj_t *line_top;
    lv_obj_t *cover;
    lv_obj_t *line_bot;
} aqi_indicator_ui_select_obj_t;

typedef struct {
    uint16_t value;
    /* value < threshold_1: green, value < threshold_2: orrange, other red */
    uint16_t threshold_1;
    uint16_t threshold_2;
    char *name;
    char *unit;
} aqi_indicator_ui_unit_data_t;

typedef struct {
    aqi_indicator_ui_unit_data_t pm1;
    aqi_indicator_ui_unit_data_t pm2_5;
    aqi_indicator_ui_unit_data_t pm10;
    aqi_indicator_ui_unit_data_t co2;
    aqi_indicator_ui_unit_data_t temp;
    aqi_indicator_ui_unit_data_t humi;
    aqi_indicator_ui_unit_data_t tvoc;
} aqi_indicator_ui_data_t;

typedef struct {
    aqi_indicator_ui_unit_data_t *data;
    const lv_img_dsc_t *image_src_green;
    const lv_img_dsc_t *image_src_orange;
    const lv_img_dsc_t *image_src_red;
} aqi_indicator_ui_select_param_t;

typedef struct {
    aqi_indicator_ui_select_t select;
    aqi_indicator_ui_select_obj_t select_obj_pre;
    aqi_indicator_ui_select_obj_t select_obj_cur;
    aqi_indicator_ui_select_param_t select_param;
    aqi_indicator_ui_data_t data;
} aqi_indicator_ui_t;

static aqi_indicator_ui_t _aqi_indicator_ui;

static void __aqi_indicator_ui_unit_data_show(aqi_indicator_ui_unit_data_t data,
                                              lv_obj_t *parent_obj, lv_obj_t *value_obj, lv_obj_t *unit_obj, lv_obj_t *bar_obj)
{
    // Set data value and unit of the label
    lv_label_set_text_fmt(value_obj, "%d", data.value);
    // lv_label_set_text_fmt(unit_obj, "%s", data.unit);

    // Update layout to get label width after data update
    // lv_obj_update_layout(parent_obj);

    // Align the value and unit label to the center of the parent object
    lv_obj_align(value_obj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(unit_obj, value_obj, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_align(parent_obj, LV_ALIGN_TOP_MID, 0, 0);

    // Set bar data value and check threshold to set level color of the value bar object
    lv_bar_set_value(bar_obj, data.value, LV_ANIM_ON);
    if (data.value < data.threshold_1)
    {
        lv_obj_set_style_bg_color(bar_obj, lv_color_hex(COLOR_GREEN), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else if (data.value < data.threshold_2)
    {
        lv_obj_set_style_bg_color(bar_obj, lv_color_hex(COLOR_ORANGE), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
    else
    {
        lv_obj_set_style_bg_color(bar_obj, lv_color_hex(COLOR_RED), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }

    lv_obj_update_layout(parent_obj);
    vTaskDelay(pdMS_TO_TICKS(20));
    lv_obj_update_layout(parent_obj);
}

static void __aqi_indicator_ui_select_param_show()
{
    lv_label_set_text_fmt(ui_LabelValueSelectAQI, "%d", _aqi_indicator_ui.select_param.data->value);

    if (_aqi_indicator_ui.select_param.data->value < _aqi_indicator_ui.select_param.data->threshold_1)
    {
        lv_img_set_src(ui_ImageLevelSelectAQI, _aqi_indicator_ui.select_param.image_src_green);
    }
    else if (_aqi_indicator_ui.select_param.data->value < _aqi_indicator_ui.select_param.data->threshold_2)
    {
        lv_img_set_src(ui_ImageLevelSelectAQI, _aqi_indicator_ui.select_param.image_src_orange);
    }
    else
    {
        lv_img_set_src(ui_ImageLevelSelectAQI, _aqi_indicator_ui.select_param.image_src_red);
    }

    lv_label_set_text(ui_LabelNameSelectAQI, _aqi_indicator_ui.select_param.data->name);
    lv_label_set_text(ui_LabelUnitSelectAQI, _aqi_indicator_ui.select_param.data->unit);
}

static void __aqi_indicator_ui_event_show()
{
    if (_aqi_indicator_ui.select_obj_pre.line_top)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_pre.line_top, LINE_OPA_SET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (_aqi_indicator_ui.select_obj_pre.cover)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_pre.cover, COVER_OPA_RESET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (_aqi_indicator_ui.select_obj_pre.line_bot)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_pre.line_bot, LINE_OPA_SET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (_aqi_indicator_ui.select_obj_cur.line_top)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_cur.line_top, LINE_OPA_RESET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (_aqi_indicator_ui.select_obj_cur.cover)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_cur.cover, COVER_OPA_SET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (_aqi_indicator_ui.select_obj_cur.line_bot)
    {
        lv_obj_set_style_bg_opa(_aqi_indicator_ui.select_obj_cur.line_bot, LINE_OPA_RESET, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    __aqi_indicator_ui_select_param_show();
}

static void __aqi_indicator_ui_event_select_set(aqi_indicator_ui_select_t select,
                                                lv_obj_t *top, lv_obj_t *cover, lv_obj_t *bot,
                                                aqi_indicator_ui_unit_data_t *data,
                                                const lv_img_dsc_t *green, const lv_img_dsc_t *orange, const lv_img_dsc_t *red)
{
    _aqi_indicator_ui.select = select;
    _aqi_indicator_ui.select_obj_pre.line_top = _aqi_indicator_ui.select_obj_cur.line_top;
    _aqi_indicator_ui.select_obj_pre.cover    = _aqi_indicator_ui.select_obj_cur.cover;
    _aqi_indicator_ui.select_obj_pre.line_bot = _aqi_indicator_ui.select_obj_cur.line_bot;
    _aqi_indicator_ui.select_obj_cur.line_top = top;
    _aqi_indicator_ui.select_obj_cur.cover    = cover;
    _aqi_indicator_ui.select_obj_cur.line_bot = bot;
    _aqi_indicator_ui.select_param.data = data;
    _aqi_indicator_ui.select_param.image_src_green  = green;
    _aqi_indicator_ui.select_param.image_src_orange = orange;
    _aqi_indicator_ui.select_param.image_src_red    = red;

    __aqi_indicator_ui_event_show();
}

void aqi_indicator_ui_init()
{
    SET_UNIT_DATA(_aqi_indicator_ui.data.pm1  , PM1_LEVEL_THRESHOLD_1  , PM1_LEVEL_THRESHOLD_2  , "PM₁"  , "µg/m³");
    SET_UNIT_DATA(_aqi_indicator_ui.data.pm2_5, PM2_5_LEVEL_THRESHOLD_1, PM2_5_LEVEL_THRESHOLD_2, "PM₂.₅", "µg/m³");
    SET_UNIT_DATA(_aqi_indicator_ui.data.pm10 , PM10_LEVEL_THRESHOLD_1 , PM10_LEVEL_THRESHOLD_2 , "PM₁₀" , "µg/m³");
    SET_UNIT_DATA(_aqi_indicator_ui.data.co2  , CO2_LEVEL_THRESHOLD_1  , CO2_LEVEL_THRESHOLD_2  , "CO₂"  , "ppm"  );
    SET_UNIT_DATA(_aqi_indicator_ui.data.temp , TEMP_LEVEL_THRESHOLD_1 , TEMP_LEVEL_THRESHOLD_2 , "TEMP" , "°C"   );
    SET_UNIT_DATA(_aqi_indicator_ui.data.humi , RH_LEVEL_THRESHOLD_1   , RH_LEVEL_THRESHOLD_2   , "RH"   , "%"    );
    SET_UNIT_DATA(_aqi_indicator_ui.data.tvoc , TVOC_LEVEL_THRESHOLD_1 , TVOC_LEVEL_THRESHOLD_2 , "TVOC" , "ppb"  );

    aqi_indicator_ui_click_event_handler(ui_ContainerCoverPM1);

    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm1,   ui_ContainerLabelDataPM1,  ui_LabelValuePM1,  ui_LabelUnitPM1,  ui_BarValuePM1);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm2_5, ui_ContainerLabelDataPM25, ui_LabelValuePM25, ui_LabelUnitPM25, ui_BarValuePM25);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm10,  ui_ContainerLabelDataPM10, ui_LabelValuePM10, ui_LabelUnitPM10, ui_BarValuePM10);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.co2,   ui_ContainerLabelDataCO2,  ui_LabelValueCO2,  ui_LabelUnitCO2,  ui_BarValueCO2);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.temp,  ui_ContainerLabelDataTEMP, ui_LabelValueTEMP, ui_LabelUnitTEMP, ui_BarValueTEMP);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.humi,  ui_ContainerLabelDataRH,   ui_LabelValueRH,   ui_LabelUnitRH,   ui_BarValueRH);
    __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.tvoc,  ui_ContainerLabelDataTVOC, ui_LabelValueTVOC, ui_LabelUnitTVOC, ui_BarValueTVOC);
}

void aqi_indicator_ui_data_show(uint16_t pm1, uint16_t pm2_5, uint16_t pm10, uint16_t co2,
                                uint16_t temp, uint16_t humi, uint16_t tvoc)
{
    if (_aqi_indicator_ui.data.pm1.value != pm1)
    {
        _aqi_indicator_ui.data.pm1.value = pm1;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm1, ui_ContainerLabelDataPM1, ui_LabelValuePM1, ui_LabelUnitPM1, ui_BarValuePM1);
    }

    if (_aqi_indicator_ui.data.pm2_5.value != pm2_5)
    {
        _aqi_indicator_ui.data.pm2_5.value = pm2_5;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm2_5, ui_ContainerLabelDataPM25, ui_LabelValuePM25, ui_LabelUnitPM25, ui_BarValuePM25);
    }

    if (_aqi_indicator_ui.data.pm10.value != pm10)
    {
        _aqi_indicator_ui.data.pm10.value = pm10;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.pm10, ui_ContainerLabelDataPM10, ui_LabelValuePM10, ui_LabelUnitPM10, ui_BarValuePM10);
    }

    if (_aqi_indicator_ui.data.co2.value != co2)
    {
        _aqi_indicator_ui.data.co2.value = co2;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.co2, ui_ContainerLabelDataCO2, ui_LabelValueCO2, ui_LabelUnitCO2, ui_BarValueCO2);
    }

    if (_aqi_indicator_ui.data.temp.value != temp)
    {
        _aqi_indicator_ui.data.temp.value = temp;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.temp, ui_ContainerLabelDataTEMP, ui_LabelValueTEMP, ui_LabelUnitTEMP, ui_BarValueTEMP);
    }

    if (_aqi_indicator_ui.data.humi.value != humi)
    {
        _aqi_indicator_ui.data.humi.value = humi;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.humi, ui_ContainerLabelDataRH , ui_LabelValueRH, ui_LabelUnitRH, ui_BarValueRH);
    }

    if (_aqi_indicator_ui.data.tvoc.value != tvoc)
    {
        _aqi_indicator_ui.data.tvoc.value = tvoc;
        __aqi_indicator_ui_unit_data_show(_aqi_indicator_ui.data.tvoc, ui_ContainerLabelDataTVOC, ui_LabelValueTVOC, ui_LabelUnitTVOC, ui_BarValueTVOC);
    }

    __aqi_indicator_ui_select_param_show();
}

void aqi_indicator_ui_click_event_handler(lv_obj_t *target)
{
    if (target == ui_ContainerCoverPM1)
	{
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_PM1,
                                            NULL, ui_ContainerCoverPM1, ui_ContainerLinePM1,
                                            &_aqi_indicator_ui.data.pm1,
                                            &ui_img_images_pm_green_png,
                                            &ui_img_images_pm_orange_png,
                                            &ui_img_images_pm_red_png);
	}
    else if (target == ui_ContainerCoverPM25)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_PM2_5,
                                            ui_ContainerLinePM1, ui_ContainerCoverPM25, ui_ContainerLinePM25,
                                            &_aqi_indicator_ui.data.pm2_5,
                                            &ui_img_images_pm_green_png,
                                            &ui_img_images_pm_orange_png,
                                            &ui_img_images_pm_red_png);
    }
    else if (target == ui_ContainerCoverPM10)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_PM10,
                                            ui_ContainerLinePM25, ui_ContainerCoverPM10, ui_ContainerLinePM10,
                                            &_aqi_indicator_ui.data.pm10,
                                            &ui_img_images_pm_green_png,
                                            &ui_img_images_pm_orange_png,
                                            &ui_img_images_pm_red_png);
    }
    else if (target == ui_ContainerCoverCO2)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_CO2,
                                            ui_ContainerLinePM10, ui_ContainerCoverCO2, NULL,
                                            &_aqi_indicator_ui.data.co2,
                                            &ui_img_images_co2_green_png,
                                            &ui_img_images_co2_orange_png,
                                            &ui_img_images_co2_red_png);
    }
    else if (target == ui_ContainerCoverTEMP)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_TEMP,
                                            NULL, ui_ContainerCoverTEMP, ui_ContainerLineTEMP,
                                            &_aqi_indicator_ui.data.temp,
                                            &ui_img_images_temp_green_png,
                                            &ui_img_images_temp_orange_png,
                                            &ui_img_images_temp_red_png);
    }
    else if (target == ui_ContainerCoverRH)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_RH,
                                            ui_ContainerLineTEMP, ui_ContainerCoverRH, ui_ContainerLineRH,
                                            &_aqi_indicator_ui.data.humi,
                                            &ui_img_images_rh_green_png,
                                            &ui_img_images_rh_orange_png,
                                            &ui_img_images_rh_red_png);
    }
    else if (target == ui_ContainerCoverTVOC)
    {
        __aqi_indicator_ui_event_select_set(AQI_INDICATOR_UI_TVOC,
                                            ui_ContainerLineRH, ui_ContainerCoverTVOC, ui_ContainerLineTVOC,
                                            &_aqi_indicator_ui.data.tvoc,
                                            &ui_img_images_tvoc_green_png,
                                            &ui_img_images_tvoc_orange_png,
                                            &ui_img_images_tvoc_red_png);
    }
    else if (target == ui_ContainerCoverSetting)
    {
        
    }
}
