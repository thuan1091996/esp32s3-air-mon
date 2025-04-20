
#ifndef _AQI_WIFI_H
#define _AQI_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "esp_event.h"

typedef enum {
    aqi_wifi_event_config       = BIT0,
    aqi_wifi_event_scan_done    = BIT1,
    aqi_wifi_event_select       = BIT2,
    aqi_wifi_event_connect      = BIT3,
    aqi_wifi_event_connected    = BIT4,
    aqi_wifi_event_disconnected = BIT5,
    aqi_wifi_event_setting_done = BIT6,
    aqi_wifi_event_all          = 0xFFFF
} aqi_wifi_event_t;

int aqi_wifi_init();

void aqi_wifi_event(aqi_wifi_event_t event);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
