
#ifndef __I2C_PORT_H__
#define __I2C_PORT_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

void* i2c_port_init(int i2c_port_num, int sda_io_num, int scl_io_num, uint8_t i2c_address, uint32_t frequency_hz);

int i2c_port_register_read(void *handle, uint8_t reg_address, uint8_t *data, size_t size);
int i2c_port_register_write(void *handle, uint8_t reg_address, uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /**< __I2C_PORT_H__ */
