#include <stdio.h>
#include <string.h>

#include "driver/i2c.h"

#include "esp_log.h"
#include "esp_err.h"

#define I2C_MASTER_TIMEOUT_MS       1000
#define TAG "I2C_PORT"

#define I2C_INTIALIZED_IN_DISPLAY

// I2C Configuration
typedef struct {
    int is_initialized;
    i2c_port_t port;
    uint8_t device_address;
} i2c_port_internal_t;

static i2c_port_internal_t _i2c_ports[I2C_NUM_MAX] = {0};

/**
 * @brief Initialize the I2C master with the given configuration
 */
void* i2c_port_init(int i2c_port_num, int sda_io_num, int scl_io_num, uint8_t i2c_address, uint32_t frequency_hz)
{
    ESP_LOGD(TAG, "i2c_port_init");

    if (i2c_port_num < 0 || i2c_port_num >= I2C_NUM_MAX)
    {
        ESP_LOGE(TAG, "Invalid I2C port number");
        return NULL;
    }

    if (!_i2c_ports[i2c_port_num].is_initialized)
    {
#if !defined(I2C_INTIALIZED_IN_DISPLAY)
        // Configure the I2C driver
        i2c_config_t config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sda_io_num,
            .scl_io_num = scl_io_num,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = frequency_hz,
        };

        // Install driver
        ESP_ERROR_CHECK(i2c_param_config(i2c_port_num, &config));
        ESP_ERROR_CHECK(i2c_driver_install(i2c_port_num, I2C_MODE_MASTER, 0, 0, 0));
#endif

        _i2c_ports[i2c_port_num].is_initialized = 1;
        _i2c_ports[i2c_port_num].port = i2c_port_num;
        _i2c_ports[i2c_port_num].device_address = i2c_address;
    }

    return &_i2c_ports[i2c_port_num];
}

/**
 * @brief Read a register from the I2C device
 */
int i2c_port_register_read(void* handle, uint8_t reg_address, uint8_t* data, size_t size)
{
    ESP_LOGD(TAG, "i2c_port_register_read");

    if (!handle || !data || size == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    i2c_port_internal_t* i2c_port = (i2c_port_internal_t*)handle;

    // Write the register address, then read the data
    esp_err_t ret = i2c_master_write_read_device(i2c_port->port, i2c_port->device_address,
                                                 &reg_address, 1, data, size, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS) );

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C read error: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

/**
 * @brief Write to a register on the I2C device
 */
int i2c_port_register_write(void* handle, uint8_t reg_address, uint8_t* data, size_t size)
{
    ESP_LOGD(TAG, "i2c_port_register_write");

    if (!handle || !data || size == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    i2c_port_internal_t* i2c_port = (i2c_port_internal_t*)handle;

    // Prepare the buffer: register address + data
    uint8_t buffer[size + 1];
    buffer[0] = reg_address;
    memcpy(&buffer[1], data, size);

    // Write the buffer to the I2C device
    esp_err_t ret = i2c_master_write_to_device(i2c_port->port, i2c_port->device_address,
                                               buffer, size + 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C write error: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}
