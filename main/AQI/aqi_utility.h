
#ifndef _AQI_UTILITY_H
#define _AQI_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

int aqi_utility_init();

int aqi_utility_update_time(struct tm timeinfo);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
