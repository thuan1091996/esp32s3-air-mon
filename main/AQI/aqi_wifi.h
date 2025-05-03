
#ifndef _AQI_WIFI_H
#define _AQI_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "esp_event.h"

typedef enum {
    aqi_wifi_event_config           = BIT0,
    aqi_wifi_event_switch_click     = BIT1,
    aqi_wifi_event_tab_switch_click = BIT2,
    aqi_wifi_event_scan_done        = BIT3,
    aqi_wifi_event_select           = BIT4,
    aqi_wifi_event_connect          = BIT5,
    aqi_wifi_event_connected        = BIT6,
    aqi_wifi_event_disconnected     = BIT7,
    aqi_wifi_event_setting_done     = BIT8,
    aqi_wifi_event_all              = 0xFFFF
} aqi_wifi_event_t;

int aqi_wifi_init();

int aqi_wifi_event(aqi_wifi_event_t event);

uint8_t aqi_wifi_exit_condition();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
