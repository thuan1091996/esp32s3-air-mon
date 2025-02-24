
#ifndef __ACP1_H__
#define __ACP1_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    union {
        uint8_t errors;
        struct {
            uint8_t too_many_fan_restarts       :1;
            uint8_t fan_speed_low               :1;
            uint8_t photodiode                  :1;
            uint8_t fan_stopped                 :1;
            uint8_t laser                       :1;
            uint8_t voc_Sensor                  :1;
            uint8_t temperature_humidity_sensor :1;
            uint8_t reserved                    :1;
        };
    };
} apc1_error_code_t;

int acp1_init();

int apc1_read_infor();
int apc1_measurement();

int apc1_set_idle_mode();
int acp1_set_deep_sleep_mode(int enable);

int apc1_set_reset_sofware();
int apc1_set_reset_hardware();

uint16_t apc1_get_pm1_0();
uint16_t apc1_get_pm2_5();
uint16_t apc1_get_pm10();
uint16_t apc1_get_pmInAir_1_0();
uint16_t apc1_get_pmInAir_2_5();
uint16_t apc1_get_pmInAir_10();
uint16_t apc1_get_noParticles_0_3();
uint16_t apc1_get_noParticles_0_5();
uint16_t apc1_get_noParticles_1_0();
uint16_t apc1_get_noParticles_2_5();
uint16_t apc1_get_noParticles_5_0();
uint16_t apc1_get_noParticles_10();
uint16_t apc1_get_TVOC();
uint16_t apc1_get_ECO2();
float apc1_get_T_comp();
float apc1_get_RH_comp();
float apc1_get_T_raw();
float apc1_get_RH_raw();
uint32_t apc1_get_RS0();
uint32_t apc1_get_RS1();
uint32_t apc1_get_RS2();
uint32_t apc1_get_RS3();
uint8_t apc1_get_AQI();
uint8_t apc1_get_FW_Version();

apc1_error_code_t apc1_get_ErrorCode();

#ifdef __cplusplus
}
#endif

#endif /**< __ACP1_H__ */
