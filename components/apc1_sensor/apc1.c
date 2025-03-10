
#include "apc1.h"
#include "apc1_define.h"

#include "i2c_port.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// TODO need to be redefined
#define ACP_SUPPORT_SET_PIN     0
#define ACP_SUPPORT_RESET_PIN   0

// TODO need to be redefined
#define APC1_I2C_MASTER_NUM     I2C_NUM_0
#define APC1_I2C_MASTER_SDA_IO  8
#define APC1_I2C_MASTER_SCL_IO  9

// TODO: need to be redefined
#if (ACP_SUPPORT_SET_PIN != 0)
#define APC1_SET_IO             0
#endif

// TODO: need to be redefined
#if (ACP_SUPPORT_RESET_PIN != 0)
#define APC1_RESET_IO           1
#endif

#define ACP1_PARSE_DATA_U8( data, resp, idx) data.idx = (uint8_t)resp[idx]
#define ACP1_PARSE_DATA_U16(data, resp, idx) data.idx = (uint16_t)((resp[idx]<<8) | resp[idx+1])
#define ACP1_PARSE_DATA_U32(data, resp, idx) data.idx = (uint32_t)((resp[idx]<<24) | (resp[idx+1]<<16) | (resp[idx+2]<<8) | resp[idx+3])

static const char *TAG = "APC1";
static void *_acp1_i2c_port_handle = NULL;

static apc1_mode_t _apc1_mode = Mode_None;

static apc1_measure_data_t _apc1_measure_data;
static apc1_sensor_infor_t _apc1_sensor_infor;

static uint16_t __apc1_checksum(uint8_t *data, size_t size)
{
    uint16_t checksum = 0;

    for (size_t i = 0; i < size; i++)
    {
        checksum += data[i];
    }

    return checksum;
}

static int __apc1_request(uint8_t command, uint8_t mode)
{
    uint16_t checksum;
    apc1_command_request_t request = {0};

    request.Frame_Header_H = Frame_Header_H;
    request.Frame_Header_L = Frame_Header_L;
    request.Command = command;
    request.Mode_H = 0x00;
    request.Mode_L = mode;
    checksum = __apc1_checksum((uint8_t*)&request, 5);
    request.Checksum_H = (checksum >> 8) & 0xFF;
    request.Checksum_L = checksum & 0xFF;

    if (i2c_port_register_write(_acp1_i2c_port_handle, Request_Register, (uint8_t*)&request, sizeof(request)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Write i2c request fail");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static int __apc1_response(uint8_t reg_address, uint8_t *data, size_t size)
{
    if ((data == NULL) || (size == 0))
    {
        ESP_LOGE(TAG, "Args invalid");
        return ESP_ERR_INVALID_ARG;
    }

    // Delay for 100 milliseconds to wait for the sensor to process the request
    vTaskDelay(pdMS_TO_TICKS(100));

    if (i2c_port_register_read(_acp1_i2c_port_handle, reg_address, data, size) != ESP_OK)
    {
        ESP_LOGE(TAG, "Read i2c response fail");
        return ESP_FAIL;
    }

    uint16_t cal_checksum = __apc1_checksum(data, size - 2);

    if (cal_checksum != (uint16_t)((data[size - 2] << 8) + data[size - 1]))
    {
        ESP_LOGE(TAG, "Response checksum fail");
        ESP_LOGE(TAG, "Calculated checksum: 0x%04X", cal_checksum);
        ESP_LOGE(TAG, "Received checksum: 0x%04X", (uint16_t)((data[size - 2] << 8) + data[size - 1]));
        //Dump the response data
        ESP_LOG_BUFFER_HEX(TAG, data, size);
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

#if (ACP_SUPPORT_SET_PIN != 0) || (ACP_SUPPORT_RESET_PIN != 0)
static int __apc1_init_io(uint64_t pin)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << pin),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };

    if (gpio_config(&io_conf) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure SET and RESET pins");
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif

int acp1_init()
{
    _acp1_i2c_port_handle = i2c_port_init(APC1_I2C_MASTER_NUM, APC1_I2C_MASTER_SDA_IO, APC1_I2C_MASTER_SCL_IO,
                                            APC1_I2C_Address, APC1_I2C_Frequency);

    if (_acp1_i2c_port_handle == NULL)
    {
        ESP_LOGE(TAG, "Create i2c_port handle fail");
        return ESP_FAIL;
    }

#if (ACP_SUPPORT_SET_PIN != 0)
    if (__apc1_init_io(APC1_SET_IO) != ESP_OK)
    {
        return ESP_FAIL;
    }
#endif

#if (ACP_SUPPORT_RESET_PIN != 0)
    if (__apc1_init_io(APC1_RESET_IO) != ESP_OK)
    {
        return ESP_FAIL;
    }
#endif

    acp1_set_deep_sleep_mode(0);
    _apc1_mode = Mode_Measurement;

    return ESP_OK;
}

int apc1_read_infor()
{
    if (__apc1_request(Command_Read_Infor, Mode_None) != ESP_OK)
    {
        ESP_LOGE(TAG, "Write infor request fail");
        return ESP_FAIL;
    }

    uint8_t response[Read_Infor_Response_Lenght];
    if (__apc1_response(Response_Infor_Register, response, sizeof(response)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Read infor response fail");
        return ESP_FAIL;
    }

    /* Parse sensor infor response */
    apc1_sensor_infor_t sensor_infor;
    // Sensor infor Name
    strncpy(sensor_infor.Name, (const char *)&response[Index_Name], LEN_Name);
    sensor_infor.Name[LEN_Name] = '\0';
    // Sensor infor Type
    sensor_infor.Type = response[Index_Type];
    // Sensor infor serial number
    memcpy(sensor_infor.Serial_Number, &response[Index_Serial_Number], LEN_Serial_Number);
    // Sensor infor firmware version
    memcpy(sensor_infor.FW_Version, &response[Index_FW_Version], LEN_FW_Version);

    _apc1_sensor_infor = sensor_infor;
#if (APC1_LOG_ENABLE != 0)
    ESP_LOGI(TAG, "Sensor infor:");
    ESP_LOGI(TAG, "Name: %s", sensor_infor.Name);
    ESP_LOGI(TAG, "Type: %d", sensor_infor.Type);
    ESP_LOGI(TAG, "Serial number: %02X%02X%02X%02X%02X%02X%02X%02X",
            sensor_infor.Serial_Number[0], sensor_infor.Serial_Number[1], sensor_infor.Serial_Number[2], sensor_infor.Serial_Number[3],
            sensor_infor.Serial_Number[4], sensor_infor.Serial_Number[5], sensor_infor.Serial_Number[6], sensor_infor.Serial_Number[7]);
    ESP_LOGI(TAG, "Firmware version: %02X%02X", sensor_infor.FW_Version[0], sensor_infor.FW_Version[1]);
#endif /* End of (APC1_LOG_ENABLE != 0) */
    return ESP_OK;
}

int apc1_measurement()
{
#if (ACP_SUPPORT_SET_PIN != 0)
    if (gpio_get_level(APC1_SET_IO) == 0)
    {
        ESP_LOGE(TAG, "Sensor is being deep sleep mode");
        return ESP_ERR_INVALID_STATE;
    }
#endif

    if (__apc1_request(Command_Set_Mode, Mode_Measurement) != ESP_OK)
    {
        ESP_LOGE(TAG, "Write measurement request fail");
        return ESP_FAIL;
    }

    uint8_t response[Set_Measurement_Mode_Response_Lenght];
    if (__apc1_response(Response_Measurement_Register, response, sizeof(response)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Read measurement response fail");
        return ESP_FAIL;
    }

    /* Parse measurement data response */
    apc1_measure_data_t measure_data;
    ACP1_PARSE_DATA_U16(measure_data, response, PM_1_0);
    ACP1_PARSE_DATA_U16(measure_data, response, PM_2_5);
    ACP1_PARSE_DATA_U16(measure_data, response, PM_10);
    ACP1_PARSE_DATA_U16(measure_data, response, PMInAir_1_0);
    ACP1_PARSE_DATA_U16(measure_data, response, PMInAir_2_5);
    ACP1_PARSE_DATA_U16(measure_data, response, PMInAir_10);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_0_3);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_0_5);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_1_0);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_2_5);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_5_0);
    ACP1_PARSE_DATA_U16(measure_data, response, NoParticles_10);
    ACP1_PARSE_DATA_U16(measure_data, response, TVOC);
    ACP1_PARSE_DATA_U16(measure_data, response, ECO2);
    ACP1_PARSE_DATA_U16(measure_data, response, T_comp);
    ACP1_PARSE_DATA_U16(measure_data, response, RH_comp);
    ACP1_PARSE_DATA_U16(measure_data, response, T_raw);
    ACP1_PARSE_DATA_U16(measure_data, response, RH_raw);
    ACP1_PARSE_DATA_U32(measure_data, response, RS0);
    ACP1_PARSE_DATA_U32(measure_data, response, RS1);
    ACP1_PARSE_DATA_U32(measure_data, response, RS2);
    ACP1_PARSE_DATA_U32(measure_data, response, RS3);
    ACP1_PARSE_DATA_U8( measure_data, response, AQI);
    ACP1_PARSE_DATA_U8( measure_data, response, FW_Version);
    ACP1_PARSE_DATA_U8( measure_data, response, ErrorCode);

    _apc1_measure_data = measure_data;

    _apc1_mode = Mode_Measurement;

#if (APC1_LOG_ENABLE != 0)
    // Logging measurement data
    ESP_LOGI(TAG, "Measurement data:");
    ESP_LOGI(TAG, "PM1.0: %d", measure_data.PM_1_0);
    ESP_LOGI(TAG, "PM2.5: %d", measure_data.PM_2_5);
    ESP_LOGI(TAG, "PM10: %d", measure_data.PM_10);
    ESP_LOGI(TAG, "PM1.0 in air: %d", measure_data.PMInAir_1_0);
    ESP_LOGI(TAG, "PM2.5 in air: %d", measure_data.PMInAir_2_5);
    ESP_LOGI(TAG, "PM10 in air: %d", measure_data.PMInAir_10);
    ESP_LOGI(TAG, "Number of particles > 0.3um: %d", measure_data.NoParticles_0_3);
    ESP_LOGI(TAG, "Number of particles > 0.5um: %d", measure_data.NoParticles_0_5);
    ESP_LOGI(TAG, "Number of particles > 1.0um: %d", measure_data.NoParticles_1_0);
    ESP_LOGI(TAG, "Number of particles > 2.5um: %d", measure_data.NoParticles_2_5);
    ESP_LOGI(TAG, "Number of particles > 5.0um: %d", measure_data.NoParticles_5_0);
    ESP_LOGI(TAG, "Number of particles > 10um: %d", measure_data.NoParticles_10);
    ESP_LOGI(TAG, "TVOC: %d", measure_data.TVOC);
    ESP_LOGI(TAG, "ECO2: %d", measure_data.ECO2);
    ESP_LOGI(TAG, "Compensated temperature: %d", measure_data.T_comp);
    ESP_LOGI(TAG, "Compensated humidity: %d", measure_data.RH_comp);
    ESP_LOGI(TAG, "Uncompensated temperature: %d", measure_data.T_raw);
    ESP_LOGI(TAG, "Uncompensated humidity: %d", measure_data.RH_raw);
    ESP_LOGI(TAG, "Gas sensor 0 raw resistance: %ld", measure_data.RS0);
    ESP_LOGI(TAG, "Gas sensor 1 raw resistance: %ld", measure_data.RS1);
    ESP_LOGI(TAG, "Gas sensor 2 raw resistance: %ld", measure_data.RS2);
    ESP_LOGI(TAG, "Gas sensor 3 raw resistance: %ld", measure_data.RS3);
    ESP_LOGI(TAG, "Air Quality Index: %d", measure_data.AQI);
    ESP_LOGI(TAG, "Firmware version: %d", measure_data.FW_Version);
    ESP_LOGI(TAG, "Error code: %d", measure_data.ErrorCode);
#endif /* End of (APC1_LOG_ENABLE != 0) */

    return ESP_OK;
}

int apc1_set_idle_mode()
{
#if (ACP_SUPPORT_SET_PIN != 0)
    if (gpio_get_level(APC1_SET_IO) == 0)
    {
        ESP_LOGE(TAG, "Sensor is being deep sleep mode");
        return ESP_ERR_INVALID_STATE;
    }
#endif

    if (__apc1_request(Command_Set_Mode, Mode_Idle) != ESP_OK)
    {
        ESP_LOGE(TAG, "Write idle mode request fail");
        return ESP_FAIL;
    }

    uint8_t response[Set_Idle_Mode_Response_Lenght];
    if (__apc1_response(Response_Idle_Register, response, sizeof(response)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Read idle mode response fail");
        return ESP_FAIL;
    }

    // Response: 0x42 0x4D 0x00 0x04 0xE4 0x00 0x01 0x77
    if ((response[4] != Command_Set_Mode) || (response[5] != Mode_Idle))
    {
        ESP_LOGE(TAG, "Data response fail");
        return ESP_FAIL;
    }

    _apc1_mode = Mode_Idle;

    return ESP_OK;
}

int apc1_set_reset_sofware()
{
    if (__apc1_request(Command_Set_Mode, Mode_Reset) != ESP_OK)
    {
        ESP_LOGE(TAG, "Write idle mode request fail");
        return ESP_FAIL;
    }

    /* Besides a power-on / reset there is no device answer. */

    return ESP_OK;
}

int apc1_set_reset_hardware()
{
#if (ACP_SUPPORT_RESET_PIN != 0)
    gpio_set_level(APC1_RESET_IO, 0);
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay for 10 milliseconds
    gpio_set_level(APC1_RESET_IO, 1);
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay for 10 milliseconds
#endif
    return ESP_OK;
}

/* enable = 0 (exit deep sleep mode), other value (Enter deep sleep mode) */
int acp1_set_deep_sleep_mode(int enable)
{
#if (ACP_SUPPORT_SET_PIN != 0)
    gpio_set_level(APC1_SET_IO, (enable == 0));
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay for 10 milliseconds
#endif
    return ESP_OK;
}

uint16_t apc1_get_pm1_0()
{
    return _apc1_measure_data.PM_1_0;
}

uint16_t apc1_get_pm2_5()
{
    return _apc1_measure_data.PM_2_5;
}

uint16_t apc1_get_pm10()
{
    return _apc1_measure_data.PM_10;
}

uint16_t apc1_get_pmInAir_1_0()
{
    return _apc1_measure_data.PMInAir_1_0;
}

uint16_t apc1_get_pmInAir_2_5()
{
    return _apc1_measure_data.PMInAir_2_5;
}

uint16_t apc1_get_pmInAir_10()
{
    return _apc1_measure_data.PMInAir_10;
}

uint16_t apc1_get_noParticles_0_3()
{
    return _apc1_measure_data.NoParticles_0_3;
}

uint16_t apc1_get_noParticles_0_5()
{
    return _apc1_measure_data.NoParticles_0_5;
}

uint16_t apc1_get_noParticles_1_0()
{
    return _apc1_measure_data.NoParticles_1_0;
}

uint16_t apc1_get_noParticles_2_5()
{
    return _apc1_measure_data.NoParticles_2_5;
}

uint16_t apc1_get_noParticles_5_0()
{
    return _apc1_measure_data.NoParticles_5_0;
}

uint16_t apc1_get_noParticles_10()
{
    return _apc1_measure_data.NoParticles_10;
}

uint16_t apc1_get_TVOC()
{
    return _apc1_measure_data.TVOC;
}

uint16_t apc1_get_ECO2()
{
    return _apc1_measure_data.ECO2;
}

float apc1_get_T_comp()
{
    return _apc1_measure_data.T_comp / Temperature_Humidity_Scale;
}

float apc1_get_RH_comp()
{
    return _apc1_measure_data.RH_comp / Temperature_Humidity_Scale;
}

float apc1_get_T_raw()
{
    return _apc1_measure_data.T_raw / Temperature_Humidity_Scale;
}

float apc1_get_RH_raw()
{
    return _apc1_measure_data.RH_raw / Temperature_Humidity_Scale;
}

uint32_t apc1_get_RS0()
{
    return _apc1_measure_data.RS0;
}

uint32_t apc1_get_RS1()
{
    return _apc1_measure_data.RS1;
}

uint32_t apc1_get_RS2()
{
    return _apc1_measure_data.RS2;
}

uint32_t apc1_get_RS3()
{
    return _apc1_measure_data.RS3;
}

uint8_t apc1_get_AQI()
{
    return _apc1_measure_data.AQI;
}

uint8_t apc1_get_FW_Version()
{
    return _apc1_measure_data.FW_Version;
}

apc1_error_code_t apc1_get_ErrorCode()
{
    return *((apc1_error_code_t*)&_apc1_measure_data.ErrorCode);
}
