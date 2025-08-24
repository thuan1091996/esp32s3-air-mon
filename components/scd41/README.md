# SCD41 ESP-IDF Component

A comprehensive ESP-IDF component for the Sensirion SCD41 CO2, temperature, and humidity sensor.

## Features

- **CO2 Measurement**: High-precision CO2 concentration measurement (400 - 40,000 ppm)
- **Temperature Measurement**: Accurate temperature measurement (-40°C to +70°C)
- **Humidity Measurement**: Reliable relative humidity measurement (0% to 100% RH)
- **I2C Communication**: Standard I2C interface with configurable port and timeout
- **CRC Validation**: Built-in CRC8 checksum validation for data integrity
- **Easy Integration**: Simple API with comprehensive error handling

## Hardware Requirements

- **Sensor**: Sensirion SCD41 CO2 sensor module
- **Interface**: I2C (3.3V logic level)
- **Connections**:
  - VDD: 3.3V
  - GND: Ground
  - SDA: I2C data line
  - SCL: I2C clock line

## Installation

### ESP Registry (Recommended)

```bash
idf.py add-dependency "chiehmin/scd41^1.0.0"
```

### Manual Installation

1. Clone or download this component
2. Place it in your project's `components` directory
3. Include the header in your code: `#include "scd41.h"`

## Quick Start

### Basic Usage

```c
#include "scd41.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SCD41_EXAMPLE";

void app_main(void)
{
    // Configure I2C master (you need to do this before using the component)
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,                // GPIO pin for SDA
        .scl_io_num = 22,                // GPIO pin for SCL
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,      // 100kHz
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    // Initialize SCD41
    scd41_config_t config = SCD41_CONFIG_DEFAULT();
    config.i2c_port = I2C_NUM_0;
    config.timeout_ms = 1000;

    ESP_ERROR_CHECK(scd41_init(&config));
    ESP_ERROR_CHECK(scd41_start_measurement());

    // Wait for first measurement (5 seconds)
    vTaskDelay(pdMS_TO_TICKS(5000));

    while (1) {
        scd41_data_t data;
        esp_err_t ret = scd41_read_measurement(&data);

        if (ret == ESP_OK && data.data_ready) {
            ESP_LOGI(TAG, "CO2: %d ppm, Temperature: %.1f°C, Humidity: %.1f%%",
                     data.co2_ppm, data.temperature, data.humidity);
        } else {
            ESP_LOGW(TAG, "Failed to read sensor data");
        }

        // SCD41 provides new data every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

## API Reference

### Configuration

#### `scd41_config_t`
Configuration structure for the SCD41 sensor.

```c
typedef struct {
    i2c_port_t i2c_port;    ///< I2C port number (I2C_NUM_0 or I2C_NUM_1)
    uint32_t timeout_ms;     ///< I2C timeout in milliseconds
} scd41_config_t;
```

#### `SCD41_CONFIG_DEFAULT()`
Macro for default configuration:
- I2C port: `I2C_NUM_0`
- Timeout: `1000ms`

### Data Structures

#### `scd41_data_t`
Structure containing sensor measurement data.

```c
typedef struct {
    uint16_t co2_ppm;        ///< CO2 concentration in parts per million
    float temperature;       ///< Temperature in degrees Celsius
    float humidity;          ///< Relative humidity in percent
    bool data_ready;         ///< Flag indicating if data is valid
} scd41_data_t;
```

### Functions

#### `esp_err_t scd41_init(const scd41_config_t *config)`
Initialize the SCD41 sensor.

**Parameters:**
- `config`: Pointer to configuration structure

**Returns:**
- `ESP_OK`: Success
- `ESP_ERR_INVALID_ARG`: Invalid configuration
- Other ESP error codes for I2C communication failures

#### `esp_err_t scd41_start_measurement(void)`
Start periodic measurement mode.

**Returns:**
- `ESP_OK`: Success
- Other ESP error codes for communication failures

#### `esp_err_t scd41_stop_measurement(void)`
Stop periodic measurement mode.

**Returns:**
- `ESP_OK`: Success
- Other ESP error codes for communication failures

#### `esp_err_t scd41_read_measurement(scd41_data_t *data)`
Read the latest measurement data.

**Parameters:**
- `data`: Pointer to data structure to store results

**Returns:**
- `ESP_OK`: Success
- `ESP_ERR_INVALID_ARG`: Null pointer
- `ESP_ERR_INVALID_CRC`: CRC validation failed
- Other ESP error codes for communication failures

#### `esp_err_t scd41_get_serial_number(uint8_t serial_number[6])`
Get the sensor's 6-byte serial number.

**Parameters:**
- `serial_number`: Buffer to store the serial number

**Returns:**
- `ESP_OK`: Success
- Other ESP error codes for communication failures

#### `esp_err_t scd41_reset(void)`
Perform a soft reset of the sensor.

**Returns:**
- `ESP_OK`: Success
- Other ESP error codes for communication failures

#### `esp_err_t scd41_data_ready(bool *ready)`
Check if new measurement data is available.

**Parameters:**
- `ready`: Pointer to boolean to store ready status

**Returns:**
- `ESP_OK`: Success
- `ESP_ERR_INVALID_ARG`: Null pointer
- Other ESP error codes for communication failures

## Technical Specifications

### Sensor Specifications
- **CO2 Range**: 400 - 40,000 ppm
- **CO2 Accuracy**: ±(40 ppm + 5% of reading)
- **Temperature Range**: -40°C to +70°C
- **Temperature Accuracy**: ±0.8°C
- **Humidity Range**: 0% to 100% RH
- **Humidity Accuracy**: ±6% RH
- **Update Rate**: 5 seconds

### Communication
- **Interface**: I2C
- **I2C Address**: 0x62 (fixed)
- **Clock Speed**: Up to 100 kHz
- **Supply Voltage**: 2.4V to 5.5V (3.3V recommended)

## Troubleshooting

### Common Issues

1. **Sensor not responding**
   - Check I2C connections (SDA, SCL, VDD, GND)
   - Verify I2C pull-up resistors (4.7kΩ recommended)
   - Ensure correct I2C port configuration

2. **CRC errors**
   - Check for electrical noise on I2C lines
   - Reduce I2C clock speed
   - Add ferrite beads or capacitors for noise filtering

3. **Inconsistent readings**
   - Allow proper warm-up time (> 1 minute)
   - Ensure adequate ventilation around sensor
   - Check for electromagnetic interference

## License

Apache License 2.0

## References

- [Sensirion SCD41 Datasheet](https://sensirion.com/products/catalog/SCD41/)
- [ESP-IDF I2C Driver Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)
