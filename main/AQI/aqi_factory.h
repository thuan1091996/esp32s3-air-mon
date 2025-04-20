#ifndef _AQI_FACTORY_H
#define _AQI_FACTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BACK = 1,
    NEXT,
} aqi_factory_event_t;

void aqi_factory_init();
void aqi_factory_button_clickable(bool clickable);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
