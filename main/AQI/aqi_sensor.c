
#include "aqi_sensor.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>

#include "esp_err.h"
#include "esp_log.h"

#include "apc1.h"
#include "scd41.h"

#include "aqi_config.h"
#include "aqi_indicator_ui.h"

#define AQI_SENSOR_TASK_NAME       "AQI Sensor Task"
#define AQI_SENSOR_TASK_STACK_SIZE (5 * 1024)
#define AQI_SENSOR_TASK_PRIORITY   (5)

#define AQI_SENSOR_CO2_TYPE        (SCD41_CO2)

#define AQI_SENSOR_SCD41_MEASURE_INTERVAL_US (5000000) // SCD41 provides new data every 5 seconds

typedef enum {
    APC1_CO2,
    SCD41_CO2
} aqi_sensor_co2_t;

typedef struct {
    uint16_t pm1_0;
    uint16_t pm2_5;
    uint16_t pm10;
    uint16_t co2;
    uint16_t t_comp;
    uint16_t rh_comp;
    uint16_t tvoc;
    uint16_t aqi;
} aqi_sensor_data_t;

typedef struct {
    aqi_config_sensor_t *config;
    aqi_sensor_co2_t co2_sensor;
    aqi_sensor_data_t data;
} aqi_sensor_t;

static const char *TAG = "AQI SENSOR";

static aqi_sensor_t _aqi_sensor = {
    .config = NULL,
    .co2_sensor = AQI_SENSOR_CO2_TYPE,
    .data = {0}
};

static int _api_sensor_scd41_measure_last = 0;

extern bool example_lvgl_lock(int timeout_ms);
extern void example_lvgl_unlock(void);

static bool __aqi_sensor_measurement(void)
{
    int apc1_ret = ESP_FAIL, scd41_ret = ESP_FAIL;

    // APC1 sensor measurement
    apc1_ret = apc1_measurement();
    if (apc1_ret == ESP_OK)
    {
        _aqi_sensor.data.pm1_0 = apc1_get_pm1_0();
        _aqi_sensor.data.pm2_5 = apc1_get_pm2_5();
        _aqi_sensor.data.pm10 = apc1_get_pm10();
        _aqi_sensor.data.t_comp = apc1_get_T_comp();
        _aqi_sensor.data.rh_comp = apc1_get_RH_comp();
        _aqi_sensor.data.tvoc = apc1_get_TVOC();
        _aqi_sensor.data.aqi = apc1_get_AQI();

        if (_aqi_sensor.co2_sensor == APC1_CO2)
        {
            _aqi_sensor.data.co2 = apc1_get_ECO2();
        }
    }
    else
    {
        ESP_LOGE(TAG, "APC1 %s", _aqi_sensor.config->error_message);
    }

    // SCD41 sensor measurement
    if ((_aqi_sensor.co2_sensor == SCD41_CO2) &&
        ((esp_timer_get_time() - _api_sensor_scd41_measure_last) > AQI_SENSOR_SCD41_MEASURE_INTERVAL_US))
    {
        scd41_data_t scd41_data;
        scd41_ret = scd41_read_measurement(&scd41_data);

        if (scd41_ret == ESP_OK)
        {
            _aqi_sensor.data.co2 = scd41_data.co2_ppm;
        }
        else
        {
            ESP_LOGE(TAG, "SCD41 %s", _aqi_sensor.config->error_message);
        }

        _api_sensor_scd41_measure_last = esp_timer_get_time();
    }

    return (apc1_ret == ESP_OK) || (scd41_ret == ESP_OK);
}

static void __aqi_sensor_measurement_task(void *pvParameters)
{
    // Initialize APC1
    acp1_init();
    apc1_read_infor();

    // Initialize SCD41
    scd41_config_t config = { .i2c_port = I2C_NUM_0, .timeout_ms = 1000 };
    scd41_init(&config);
    scd41_start_measurement();
    _api_sensor_scd41_measure_last = esp_timer_get_time();

    while (true)
    {
        if (__aqi_sensor_measurement())
        {
            example_lvgl_lock(-1);
            aqi_indicator_ui_data_show( _aqi_sensor.data.pm1_0,
                                        _aqi_sensor.data.pm2_5,
                                        _aqi_sensor.data.pm10,
                                        _aqi_sensor.data.co2,
                                        _aqi_sensor.data.t_comp,
                                        _aqi_sensor.data.rh_comp,
                                        _aqi_sensor.data.tvoc,
                                        _aqi_sensor.data.aqi);
            example_lvgl_unlock();
        }

        vTaskDelay(pdMS_TO_TICKS(_aqi_sensor.config->period_ms));
    }
}

void aqi_sensor_init()
{
    _aqi_sensor.config = aqi_config_sensor_get();

    xTaskCreate(&__aqi_sensor_measurement_task, AQI_SENSOR_TASK_NAME,
                AQI_SENSOR_TASK_STACK_SIZE, NULL, AQI_SENSOR_TASK_PRIORITY, NULL);
}
