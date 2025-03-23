
#ifndef _AQI_INDICATOR_UI_DEFINE_H
#define _AQI_INDICATOR_UI_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#define AQI_INDICATOR_NUM 7

/* 
 * Constants for the AQI Indicator
 */
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
 * Min and max values as in the data sheet of ACP1 sensor
 */
#define PM1_VALUE_MIN           0
#define PM1_LEVEL_THRESHOLD_1   300
#define PM1_LEVEL_THRESHOLD_2   400
#define PM1_VALUE_MAX           500
#define PM1_NAME                "PM₁"
#define PM1_UNIT                "µg/m³"
#define PM1_LINE_TOP            NULL
#define PM1_LINE_BOT            &ui_ContainerLinePM1
#define PM1_IMAGE_GREEN         ui_img_images_pm_green_png
#define PM1_IMAGE_ORANGE        ui_img_images_pm_orange_png
#define PM1_IMAGE_RED           ui_img_images_pm_red_png

#define PM25_VALUE_MIN          0
#define PM25_LEVEL_THRESHOLD_1  500
#define PM25_LEVEL_THRESHOLD_2  800
#define PM25_VALUE_MAX          1000
#define PM25_NAME               "PM₂.₅"
#define PM25_UNIT               "µg/m³"
#define PM25_LINE_TOP           &ui_ContainerLinePM1
#define PM25_LINE_BOT           &ui_ContainerLinePM25
#define PM25_IMAGE_GREEN        ui_img_images_pm_green_png
#define PM25_IMAGE_ORANGE       ui_img_images_pm_orange_png
#define PM25_IMAGE_RED          ui_img_images_pm_red_png

#define PM10_VALUE_MIN          0
#define PM10_LEVEL_THRESHOLD_1  800
#define PM10_LEVEL_THRESHOLD_2  1300
#define PM10_VALUE_MAX          1500
#define PM10_NAME               "PM₁₀"
#define PM10_UNIT               "µg/m³"
#define PM10_LINE_TOP           &ui_ContainerLinePM25
#define PM10_LINE_BOT           &ui_ContainerLinePM10
#define PM10_IMAGE_GREEN        ui_img_images_pm_green_png
#define PM10_IMAGE_ORANGE       ui_img_images_pm_orange_png
#define PM10_IMAGE_RED          ui_img_images_pm_red_png

#define CO2_VALUE_MIN           400
#define CO2_LEVEL_THRESHOLD_1   30000
#define CO2_LEVEL_THRESHOLD_2   50000
#define CO2_VALUE_MAX           65000
#define CO2_NAME                "CO₂"
#define CO2_UNIT                "ppm"
#define CO2_LINE_TOP            &ui_ContainerLinePM10
#define CO2_LINE_BOT            NULL
#define CO2_IMAGE_GREEN         ui_img_images_co2_green_png
#define CO2_IMAGE_ORANGE        ui_img_images_co2_orange_png
#define CO2_IMAGE_RED           ui_img_images_co2_red_png

#define TEMP_VALUE_MIN          0
#define TEMP_LEVEL_THRESHOLD_1  25
#define TEMP_LEVEL_THRESHOLD_2  40
#define TEMP_VALUE_MAX          50
#define TEMP_NAME               "TEMP"
#define TEMP_UNIT               "°C"
#define TEMP_LINE_TOP           NULL
#define TEMP_LINE_BOT           &ui_ContainerLineTEMP
#define TEMP_IMAGE_GREEN        ui_img_images_temp_green_png
#define TEMP_IMAGE_ORANGE       ui_img_images_temp_orange_png
#define TEMP_IMAGE_RED          ui_img_images_temp_red_png

#define RH_VALUE_MIN            0
#define RH_LEVEL_THRESHOLD_1    50
#define RH_LEVEL_THRESHOLD_2    80
#define RH_VALUE_MAX            100
#define RH_NAME                 "RH"
#define RH_UNIT                 "%"
#define RH_LINE_TOP             &ui_ContainerLineTEMP
#define RH_LINE_BOT             &ui_ContainerLineRH
#define RH_IMAGE_GREEN          ui_img_images_rh_green_png
#define RH_IMAGE_ORANGE         ui_img_images_rh_orange_png
#define RH_IMAGE_RED            ui_img_images_rh_red_png

#define TVOC_VALUE_MIN          0
#define TVOC_LEVEL_THRESHOLD_1  30000
#define TVOC_LEVEL_THRESHOLD_2  50000
#define TVOC_VALUE_MAX          65000
#define TVOC_NAME               "TVOC"
#define TVOC_UNIT               "ppb"
#define TVOC_LINE_TOP           &ui_ContainerLineRH
#define TVOC_LINE_BOT           &ui_ContainerLineTVOC
#define TVOC_IMAGE_GREEN        ui_img_images_tvoc_green_png
#define TVOC_IMAGE_ORANGE       ui_img_images_tvoc_orange_png
#define TVOC_IMAGE_RED          ui_img_images_tvoc_red_png

#define ATTRIBUTE_INIT(AQI)\
{\
    .data = {\
        .value = 0,\
        .threshold_1 = AQI##_LEVEL_THRESHOLD_1,\
        .threshold_2 = AQI##_LEVEL_THRESHOLD_2,\
        .name = AQI##_NAME,\
        .unit = AQI##_UNIT\
    },\
    .ui = {\
        .container = &ui_ContainerLabelData##AQI,\
        .value_label = &ui_LabelValue##AQI,\
        .unit_label = &ui_LabelUnit##AQI,\
        .bar = &ui_BarValue##AQI,\
        .line_top = AQI##_LINE_TOP,\
        .line_bot = AQI##_LINE_BOT,\
        .click_container_cover = &ui_ContainerCover##AQI,\
        .click_panel = &ui_Panel##AQI,\
        .image_green = &AQI##_IMAGE_GREEN,\
        .image_orange = &AQI##_IMAGE_ORANGE,\
        .image_red = &AQI##_IMAGE_RED\
    }\
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
