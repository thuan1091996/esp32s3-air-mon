
#ifndef _AQI_INDICATOR_UI_DEFINE_H
#define _AQI_INDICATOR_UI_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aqi_config.h"

/*
 * Number of the AQI Indicator
 */
#define AQI_INDICATOR_NUM AQI_CONFIG_INDICATOR_NUM

/* 
 * Constants for the AQI Indicator
 */
#define AQI_INDICATOR_COVER_OPA_SET   82
#define AQI_INDICATOR_COVER_OPA_RESET 0
#define AQI_INDICATOR_LINE_OPA_SET    255
#define AQI_INDICATOR_LINE_OPA_RESET  0

#define AQI_INDICATOR_UNIT_ALIGN_TO_LABEL_OFFSET 3
#define AQI_INDICATOR_CONTAINER_STATUS_SIZE      374, 47

/* 
 * Constants for the AQI Indicator UI
 */
#define PM1_LINE_TOP  NULL
#define PM1_LINE_BOT  &ui_ContainerLinePM1

#define PM25_LINE_TOP &ui_ContainerLinePM1
#define PM25_LINE_BOT &ui_ContainerLinePM25

#define PM10_LINE_TOP &ui_ContainerLinePM25
#define PM10_LINE_BOT &ui_ContainerLinePM10

#define CO2_LINE_TOP  &ui_ContainerLinePM10
#define CO2_LINE_BOT  NULL

#define TEMP_LINE_TOP NULL
#define TEMP_LINE_BOT &ui_ContainerLineTEMP

#define RH_LINE_TOP   &ui_ContainerLineTEMP
#define RH_LINE_BOT   &ui_ContainerLineRH

#define TVOC_LINE_TOP &ui_ContainerLineRH
#define TVOC_LINE_BOT &ui_ContainerLineTVOC

#define AQI_LINE_TOP &ui_ContainerLineTVOC
#define AQI_LINE_BOT NULL

/*
 * AQI Indicator UI Attribute default values
 */
#define ATTRIBUTE_DEFAULT(AQI, aqi)\
{\
    .ui = {\
        .value_label = &ui_LabelValue##AQI,\
        .unit_label = &ui_LabelUnit##AQI,\
        .bar = &ui_BarValue##AQI,\
        .line_top = AQI##_LINE_TOP,\
        .line_bot = AQI##_LINE_BOT,\
        .click_container_cover = &ui_ContainerCover##AQI,\
        .click_panel = &ui_Panel##AQI,\
        .image_good = &ui_img_images_##aqi##_good_png,\
        .image_warning = &ui_img_images_##aqi##_warning_png,\
        .image_bad = &ui_img_images_##aqi##_bad_png\
    }\
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
