
#ifndef __ACP1_H__
#define __ACP1_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "apc1_define.h"
#ifdef __cplusplus
extern "C" {
#endif

int acp1_init();

int apc1_read_infor();
int apc1_measurement();

int apc1_set_idle_mode();
int acp1_set_deep_sleep_mode(int enable);

int apc1_set_reset_sofware();
int apc1_set_reset_hardware();

#ifdef __cplusplus
}
#endif

#endif /**< __ACP1_H__ */
