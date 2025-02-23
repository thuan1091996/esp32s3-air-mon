
#include "i2c_port.h"

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_TIMEOUT_MS       1000

typedef struct {
    int is_new_bus;
    i2c_master_bus_handle_t bus_handle;
} i2c_port_internal_t;

static const char *TAG = "I2C_PORT";
static i2c_port_internal_t _i2c_port[I2C_NUM_MAX] = {0};

void* i2c_port_init(int i2c_port_num, int sda_io_num, int scl_io_num, uint8_t i2c_address, uint32_t frequency_hz)
{
    if ((i2c_port_num < 0) || (i2c_port_num >= I2C_NUM_MAX))
    {
        ESP_LOGE(TAG, "i2c_port_num arg invalid");
        return NULL;
    }

    i2c_master_dev_handle_t *handle = malloc(sizeof(i2c_master_dev_handle_t));
    if (handle == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return NULL;
    }

    if (_i2c_port[i2c_port_num].is_new_bus == 0)
    {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = i2c_port_num,
            .sda_io_num = sda_io_num,
            .scl_io_num = scl_io_num,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };
        if (i2c_new_master_bus(&bus_config, &_i2c_port[i2c_port_num].bus_handle) != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to i2c_new_master_bus");
            return NULL;
        }
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_address,
        .scl_speed_hz = frequency_hz,
    };
    if (i2c_master_bus_add_device(_i2c_port[i2c_port_num].bus_handle, &dev_config, handle) != ESP_OK)
    {
        free(handle);
        i2c_del_master_bus(_i2c_port[i2c_port_num].bus_handle);
        ESP_LOGE(TAG, "Failed to i2c_master_bus_add_device");
        return NULL;
    }

    _i2c_port[i2c_port_num].is_new_bus = 1;

    return handle;
}

int i2c_port_register_read(void *handle, uint8_t reg_address, uint8_t *data, size_t size)
{
    if ((handle == NULL) || (data == NULL) || (size == 0))
    {
        ESP_LOGE(TAG, "Args invalid");
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_transmit_receive(handle, &reg_address, 1, data, size, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

int i2c_port_register_write(void *handle, uint8_t reg_address, uint8_t *data, size_t size)
{
    if ((handle == NULL) || (data == NULL) || (size == 0))
    {
        ESP_LOGE(TAG, "Args invalid");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buffer[size + 1];
    buffer[0] = reg_address;
    memcpy(&buffer[1], data, size);

    return i2c_master_transmit(handle, buffer, size + 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}
