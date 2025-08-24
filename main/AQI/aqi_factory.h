#ifndef _AQI_FACTORY_H
#define _AQI_FACTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** 
 * TODO: If this macro is defined, the device only configures wifi during boot and works normally.
 *       Otherwise, it configures all settings.
 */
#define AQI_FACTORY_ONLY_CONFIG_WIFI

void aqi_factory_init();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
