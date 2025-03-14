// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.11
// Project name: esp32s3-air-mon

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////


// SCREEN: ui_ScreenLoadAQI
void ui_ScreenLoadAQI_screen_init(void);
void ui_event_ScreenLoadAQI(lv_event_t * e);
lv_obj_t * ui_ScreenLoadAQI;
lv_obj_t * ui_ContainerLoad;
lv_obj_t * ui_ContainerLoadStatus;
lv_obj_t * ui_ContaierLoadLeftSide;
lv_obj_t * ui_LabelLoadTime;
lv_obj_t * ui_ContainerLoadRightSide;
lv_obj_t * ui_ContainerLoadMobileSignal;
lv_obj_t * ui_ImageLoadMobileSignal;
lv_obj_t * ui_ContainerLoadWifi;
lv_obj_t * ui_ImageLoadWifi;
lv_obj_t * ui_ContainerLoadBattery;
lv_obj_t * ui_ImageLoadBattery;
lv_obj_t * ui_BarLoadBattery;
lv_obj_t * ui_ContainerLoadMain;
lv_obj_t * ui_SpinnerLoadProgress;
lv_obj_t * ui_ContainerLoading;
lv_obj_t * ui_ContainerLoadName;
lv_obj_t * ui_ImageLoadName;
lv_obj_t * ui_LabelLoading;
// CUSTOM VARIABLES


// SCREEN: ui_ScreenIndicatorAQI
void ui_ScreenIndicatorAQI_screen_init(void);
lv_obj_t * ui_ScreenIndicatorAQI;
lv_obj_t * ui_ContainerRegionLeft;
void ui_event_ContainerCoverPM1(lv_event_t * e);
lv_obj_t * ui_ContainerCoverPM1;
void ui_event_PanelPM1(lv_event_t * e);
lv_obj_t * ui_PanelPM1;
lv_obj_t * ui_ContainerPM1;
lv_obj_t * ui_ImageIconPM1;
lv_obj_t * ui_ContainerDataPM1;
lv_obj_t * ui_ContainerLabelDataPM1;
lv_obj_t * ui_LabelValuePM1;
lv_obj_t * ui_LabelUnitPM1;
lv_obj_t * ui_BarValuePM1;
lv_obj_t * ui_LabelNamePM1;
void ui_event_ContainerLinePM1(lv_event_t * e);
lv_obj_t * ui_ContainerLinePM1;
void ui_event_ContainerCoverPM25(lv_event_t * e);
lv_obj_t * ui_ContainerCoverPM25;
void ui_event_PanelPM25(lv_event_t * e);
lv_obj_t * ui_PanelPM25;
lv_obj_t * ui_ContainerPM25;
lv_obj_t * ui_ImageIconPM25;
lv_obj_t * ui_ContainerDataPM25;
lv_obj_t * ui_ContainerLabelDataPM25;
lv_obj_t * ui_LabelValuePM25;
lv_obj_t * ui_LabelUnitPM25;
lv_obj_t * ui_BarValuePM25;
lv_obj_t * ui_LabelNamePM25;
void ui_event_ContainerLinePM25(lv_event_t * e);
lv_obj_t * ui_ContainerLinePM25;
void ui_event_ContainerCoverPM10(lv_event_t * e);
lv_obj_t * ui_ContainerCoverPM10;
void ui_event_PanelPM10(lv_event_t * e);
lv_obj_t * ui_PanelPM10;
lv_obj_t * ui_ContainerPM10;
lv_obj_t * ui_ImageIconPM10;
lv_obj_t * ui_ContainerDataPM10;
lv_obj_t * ui_ContainerLabelDataPM10;
lv_obj_t * ui_LabelValuePM10;
lv_obj_t * ui_LabelUnitPM10;
lv_obj_t * ui_BarValuePM10;
lv_obj_t * ui_LabelNamePM10;
void ui_event_ContainerLinePM10(lv_event_t * e);
lv_obj_t * ui_ContainerLinePM10;
void ui_event_ContainerCoverCO2(lv_event_t * e);
lv_obj_t * ui_ContainerCoverCO2;
void ui_event_PanelCO2(lv_event_t * e);
lv_obj_t * ui_PanelCO2;
lv_obj_t * ui_ContainerCO2;
lv_obj_t * ui_ImageIconCO2;
lv_obj_t * ui_ContainerDataCO2;
lv_obj_t * ui_ContainerLabelDataCO2;
lv_obj_t * ui_LabelValueCO2;
lv_obj_t * ui_LabelUnitCO2;
lv_obj_t * ui_BarValueCO2;
lv_obj_t * ui_LabelNameCO2;
lv_obj_t * ui_PanelLineCenterLeft;
lv_obj_t * ui_ContainerRegionCenter;
lv_obj_t * ui_ContainerStatus;
lv_obj_t * ui_ContaierLeftSide;
lv_obj_t * ui_LabelTime;
lv_obj_t * ui_ContainerRightSide;
lv_obj_t * ui_ContainerMobileSignal;
lv_obj_t * ui_ImageMobileSignal;
lv_obj_t * ui_ContainerWifi;
lv_obj_t * ui_ImageWifi;
lv_obj_t * ui_ContainerBattery;
lv_obj_t * ui_ImageBattery;
lv_obj_t * ui_BarBattery;
lv_obj_t * ui_ContainerMain;
lv_obj_t * ui_ContainerSelectAQI;
lv_obj_t * ui_LabelValueSelectAQI;
lv_obj_t * ui_ContaierInforSelectAQI;
lv_obj_t * ui_ContainerLevelSelectAQI;
lv_obj_t * ui_ContainerAlignLevelSelectAQI;
lv_obj_t * ui_ImageLevelSelectAQI;
lv_obj_t * ui_LabelUnitSelectAQI;
lv_obj_t * ui_ContainerNameSelectAQI;
lv_obj_t * ui_LabelNameSelectAQI;
lv_obj_t * ui_PanelLineCenterRight;
lv_obj_t * ui_ContainerRegionRight;
void ui_event_ContainerCoverTEMP(lv_event_t * e);
lv_obj_t * ui_ContainerCoverTEMP;
void ui_event_PanelTEMP(lv_event_t * e);
lv_obj_t * ui_PanelTEMP;
lv_obj_t * ui_ContainerTEMP;
lv_obj_t * ui_ImageIconTEMP;
lv_obj_t * ui_ContainerDataTEMP;
lv_obj_t * ui_ContainerLabelDataTEMP;
lv_obj_t * ui_LabelValueTEMP;
lv_obj_t * ui_LabelUnitTEMP;
lv_obj_t * ui_BarValueTEMP;
lv_obj_t * ui_LabelNameTEMP;
void ui_event_ContainerLineTEMP(lv_event_t * e);
lv_obj_t * ui_ContainerLineTEMP;
void ui_event_ContainerCoverRH(lv_event_t * e);
lv_obj_t * ui_ContainerCoverRH;
void ui_event_PanelRH(lv_event_t * e);
lv_obj_t * ui_PanelRH;
lv_obj_t * ui_ContainerRH;
lv_obj_t * ui_ImageIconRH;
lv_obj_t * ui_ContainerDataRH;
lv_obj_t * ui_ContainerLabelDataRH;
lv_obj_t * ui_LabelValueRH;
lv_obj_t * ui_LabelUnitRH;
lv_obj_t * ui_BarValueRH;
lv_obj_t * ui_LabelNameRH;
void ui_event_ContainerLineRH(lv_event_t * e);
lv_obj_t * ui_ContainerLineRH;
void ui_event_ContainerCoverTVOC(lv_event_t * e);
lv_obj_t * ui_ContainerCoverTVOC;
void ui_event_PanelTVOC(lv_event_t * e);
lv_obj_t * ui_PanelTVOC;
lv_obj_t * ui_ContainerTVOC;
lv_obj_t * ui_ImageIconTVOC;
lv_obj_t * ui_ContainerDataTVOC;
lv_obj_t * ui_ContainerLabelDataTVOC;
lv_obj_t * ui_LabelValueTVOC;
lv_obj_t * ui_LabelUnitTVOC;
lv_obj_t * ui_BarValueTVOC;
lv_obj_t * ui_LabelNameTVOC;
void ui_event_ContainerLineTVOC(lv_event_t * e);
lv_obj_t * ui_ContainerLineTVOC;
void ui_event_ContainerCoverSetting(lv_event_t * e);
lv_obj_t * ui_ContainerCoverSetting;
void ui_event_PanelSetting(lv_event_t * e);
lv_obj_t * ui_PanelSetting;
lv_obj_t * ui_ContainerSetting;
lv_obj_t * ui_ImageIconSetting;
// CUSTOM VARIABLES

// EVENTS
lv_obj_t * ui____initial_actions0;

// IMAGES AND IMAGE SETS
const lv_img_dsc_t * ui_imgset_mobilesignal[1] = {&ui_img_images_mobilesignal4_png};
const lv_img_dsc_t * ui_imgset_setting_[1] = {&ui_img_images_setting32_32_png};
const lv_img_dsc_t * ui_imgset_wifi[3] = {&ui_img_images_wifi1_png, &ui_img_images_wifi2_png, &ui_img_images_wifi3_png};

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_ScreenLoadAQI(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOADED) {
        _ui_screen_change(&ui_ScreenIndicatorAQI, LV_SCR_LOAD_ANIM_FADE_ON, 500, 1000, &ui_ScreenIndicatorAQI_screen_init);
    }
}

void ui_event_ContainerCoverPM1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm1_click_event(e);
    }
}

void ui_event_PanelPM1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm1_click_event(e);
    }
}

void ui_event_ContainerLinePM1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm1_click_event(e);
    }
}

void ui_event_ContainerCoverPM25(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm2_5_click_event(e);
    }
}

void ui_event_PanelPM25(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm2_5_click_event(e);
    }
}

void ui_event_ContainerLinePM25(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm2_5_click_event(e);
    }
}

void ui_event_ContainerCoverPM10(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm10_click_event(e);
    }
}

void ui_event_PanelPM10(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm10_click_event(e);
    }
}

void ui_event_ContainerLinePM10(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_pm10_click_event(e);
    }
}

void ui_event_ContainerCoverCO2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_co2_click_event(e);
    }
}

void ui_event_PanelCO2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_co2_click_event(e);
    }
}

void ui_event_ContainerCoverTEMP(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_temp_click_event(e);
    }
}

void ui_event_PanelTEMP(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_temp_click_event(e);
    }
}

void ui_event_ContainerLineTEMP(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_temp_click_event(e);
    }
}

void ui_event_ContainerCoverRH(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_rh_click_event(e);
    }
}

void ui_event_PanelRH(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_rh_click_event(e);
    }
}

void ui_event_ContainerLineRH(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_rh_click_event(e);
    }
}

void ui_event_ContainerCoverTVOC(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_tvoc_click_event(e);
    }
}

void ui_event_PanelTVOC(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_tvoc_click_event(e);
    }
}

void ui_event_ContainerLineTVOC(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_tvoc_click_event(e);
    }
}

void ui_event_ContainerCoverSetting(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_setting_click_event(e);
    }
}

void ui_event_PanelSetting(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        aqi_indicator_setting_click_event(e);
    }
}

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_ScreenLoadAQI_screen_init();
    ui_ScreenIndicatorAQI_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_ScreenLoadAQI);
}
