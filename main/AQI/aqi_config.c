
#include "aqi_config.h"
#include "aqi_config_define.h"

#include "ui.h"

#include "esp_spiffs.h"
#include "cJSON.h"

#include "esp_err.h"
#include "esp_log.h"

typedef struct {
    aqi_config_theme_t theme;
    aqi_config_sensor_t sensor;
    aqi_config_indicator_t indicator[AQI_CONFIG_INDICATOR_NUM];
} aqi_config_t;

static const char *TAG = "AQI CONFIG";

static aqi_config_t _aqi_config = AQI_CONFIG_DEFAULT();

char* __aqi_config_json_pack(aqi_config_t config)
{
    // Create a JSON object
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    // Add theme configuration
    cJSON *theme = cJSON_CreateObject();
    if (theme)
    {
        cJSON_AddItemToObject(root, AQI_CONFIG_THEME_KEY, theme);
        cJSON_AddNumberToObject(theme, AQI_CONFIG_THEME_KEY_COLOR_PRIMARY, config.theme.color_primary);
        cJSON_AddNumberToObject(theme, AQI_CONFIG_THEME_KEY_COLOR_SECONDARY, config.theme.color_secondary);
        cJSON_AddBoolToObject(theme, AQI_CONFIG_THEME_KEY_DARK_MODE, config.theme.dark_mode);
    }

    // Add sensor configuration
    cJSON *sensor = cJSON_CreateObject();
    if (sensor)
    {
        cJSON_AddItemToObject(root, AQI_CONFIG_SENSOR_KEY, sensor);
        cJSON_AddNumberToObject(sensor, AQI_CONFIG_SENSOR_KEY_PERIOD_MS, config.sensor.period_ms);
        cJSON_AddStringToObject(sensor, AQI_CONFIG_SENSOR_KEY_ERROR_MESSAGE, config.sensor.error_message);
    }

    // Add indicators configuration
    cJSON *indicators = cJSON_CreateArray();
    if (indicators)
    {
        cJSON_AddItemToObject(root, AQI_CONFIG_INDICATOR_KEY, indicators);
        for (int i = 0; i < AQI_CONFIG_INDICATOR_NUM; i++)
        {
            cJSON *indicator = cJSON_CreateObject();
            if (indicator)
            {
                cJSON_AddItemToArray(indicators, indicator);
                cJSON_AddNumberToObject(indicator, AQI_CONFIG_INDICATOR_KEY_THRESHOLD_WARNING, config.indicator[i].threshold_warning);
                cJSON_AddNumberToObject(indicator, AQI_CONFIG_INDICATOR_KEY_THRESHOLD_BAD, config.indicator[i].threshold_bad);
                cJSON_AddNumberToObject(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_GOOD, config.indicator[i].color_good);
                cJSON_AddNumberToObject(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_WARNING, config.indicator[i].color_warning);
                cJSON_AddNumberToObject(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_BAD, config.indicator[i].color_bad);
                cJSON_AddStringToObject(indicator, AQI_CONFIG_INDICATOR_KEY_NAME, config.indicator[i].name);
                cJSON_AddStringToObject(indicator, AQI_CONFIG_INDICATOR_KEY_UNIT, config.indicator[i].unit);
            }
        }
    }

    // Serialize JSON object to string
    char *json_string = cJSON_PrintUnformatted(root);
    if (json_string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
    }
    else
    {
        ESP_LOGI(TAG, "JSON packed successfully: %lld\n%s", strlen(json_string), json_string);
    }

    // Free the JSON object (frees all child objects too)
    cJSON_Delete(root);

    return json_string;
}

int __aqi_config_json_parse(const char *json_string, aqi_config_t *config)
{
    if (json_string == NULL || config == NULL)
    {
        ESP_LOGE(TAG, "Invalid input to parse function");
        return ESP_FAIL;
    }

    // Parse the JSON string
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON string");
        return ESP_FAIL;
    }

    // Parse theme configuration
    cJSON *theme = cJSON_GetObjectItem(root, AQI_CONFIG_THEME_KEY);
    if (theme)
    {
        cJSON *color_primary = cJSON_GetObjectItem(theme, AQI_CONFIG_THEME_KEY_COLOR_PRIMARY);
        cJSON *color_secondary = cJSON_GetObjectItem(theme, AQI_CONFIG_THEME_KEY_COLOR_SECONDARY);
        cJSON *dark_mode = cJSON_GetObjectItem(theme, AQI_CONFIG_THEME_KEY_DARK_MODE);

        config->theme.color_primary = color_primary ? color_primary->valueint : 0;
        config->theme.color_secondary = color_secondary ? color_secondary->valueint : 0;
        config->theme.dark_mode = dark_mode ? cJSON_IsTrue(dark_mode) : false;
    }

    // Parse sensor configuration
    cJSON *sensor = cJSON_GetObjectItem(root, AQI_CONFIG_SENSOR_KEY);
    if (sensor)
    {
        cJSON *period_ms = cJSON_GetObjectItem(sensor, AQI_CONFIG_SENSOR_KEY_PERIOD_MS);
        cJSON *error_message = cJSON_GetObjectItem(sensor, AQI_CONFIG_SENSOR_KEY_ERROR_MESSAGE);

        config->sensor.period_ms = period_ms ? period_ms->valueint : 0;
        if (error_message && cJSON_IsString(error_message))
        {
            strncpy(config->sensor.error_message, error_message->valuestring, sizeof(config->sensor.error_message) - 1);
        }
        else
        {
            config->sensor.error_message[0] = '\0';
        }
    }

    // Parse indicators configuration
    cJSON *indicators = cJSON_GetObjectItem(root, AQI_CONFIG_INDICATOR_KEY);
    if (indicators && cJSON_IsArray(indicators))
    {
        int indicator_count = cJSON_GetArraySize(indicators);
        if (indicator_count > AQI_CONFIG_INDICATOR_NUM)
        {
            ESP_LOGW(TAG, "Number of indicators exceeds limit");
            indicator_count = AQI_CONFIG_INDICATOR_NUM;
        }

        for (int i = 0; i < indicator_count; i++)
        {
            cJSON *indicator = cJSON_GetArrayItem(indicators, i);
            if (indicator)
            {
                cJSON *threshold_warning = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_THRESHOLD_WARNING);
                cJSON *threshold_bad = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_THRESHOLD_BAD);
                cJSON *color_good = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_GOOD);
                cJSON *color_warning = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_WARNING);
                cJSON *color_bad = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_COLOR_BAD);
                cJSON *name = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_NAME);
                cJSON *unit = cJSON_GetObjectItem(indicator, AQI_CONFIG_INDICATOR_KEY_UNIT);

                config->indicator[i].threshold_warning = threshold_warning ? threshold_warning->valueint : 0;
                config->indicator[i].threshold_bad = threshold_bad ? threshold_bad->valueint : 0;
                config->indicator[i].color_good = color_good ? color_good->valueint : 0;
                config->indicator[i].color_warning = color_warning ? color_warning->valueint : 0;
                config->indicator[i].color_bad = color_bad ? color_bad->valueint : 0;

                if (name && cJSON_IsString(name))
                    strncpy(config->indicator[i].name, name->valuestring, sizeof(config->indicator[i].name) - 1);
                else
                    config->indicator[i].name[0] = '\0';

                if (unit && cJSON_IsString(unit))
                    strncpy(config->indicator[i].unit, unit->valuestring, sizeof(config->indicator[i].unit) - 1);
                else
                    config->indicator[i].unit[0] = '\0';
            }
        }
    }

    // Free the JSON root object
    cJSON_Delete(root);

    ESP_LOGI(TAG, "JSON parsed successfully");
    return ESP_OK;
}

int __aqi_config_save_to_spiffs(aqi_config_t *config)
{
    esp_err_t ret = ESP_OK;
    char *json_string = __aqi_config_json_pack(*config);

    if (json_string == NULL)
    {
        ESP_LOGE(TAG, "Failed to pack JSON");
        return ESP_FAIL;
    }

    // Write JSON string to file
    FILE *file = fopen(AQI_CONFIG_SPIFFS_FILE_PATH, "w");
    if (file)
    {
        fprintf(file, "%s", json_string);
        fclose(file);
        ESP_LOGI(TAG, "Configuration saved successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        ret = ESP_FAIL;
    }

    free(json_string);
    return ret;
}

int __aqi_config_load_from_spiffs(aqi_config_t *config)
{
    esp_err_t ret = ESP_FAIL;
    FILE *file = fopen(AQI_CONFIG_SPIFFS_FILE_PATH, "r");

    if (file)
    {
        // Read file content into a buffer
        char *buffer = malloc(AQI_CONFIG_SPIFFS_MAX_SIZE);
        if (buffer == NULL)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for buffer");
            fclose(file);
            return ESP_FAIL;
        }

        buffer[fread(buffer, 1, AQI_CONFIG_SPIFFS_MAX_SIZE - 1, file)] = '\0';
        fclose(file);

        ret = __aqi_config_json_parse(buffer, config);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Configuration loaded successfully");
        }

        free(buffer);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
    }

    return ret;
}

void __aqi_config_show_debug(const aqi_config_t *config)
{
    if (config == NULL)
    {
        ESP_LOGE("AQI_CONFIG", "Config is NULL");
        return;
    }

    // Print the theme configuration
    ESP_LOGI("AQI_CONFIG", "Applying Theme Configuration:");
    ESP_LOGI("AQI_CONFIG", "    Primary Color: %d", config->theme.color_primary);
    ESP_LOGI("AQI_CONFIG", "    Secondary Color: %d", config->theme.color_secondary);
    ESP_LOGI("AQI_CONFIG", "    Dark Mode: %s", config->theme.dark_mode ? "Enabled" : "Disabled");

    // Print the sensor configuration
    ESP_LOGI("AQI_CONFIG", "Applying Sensor Configuration:");
    ESP_LOGI("AQI_CONFIG", "    Period (ms): %d", config->sensor.period_ms);
    ESP_LOGI("AQI_CONFIG", "    Error Message: %s", config->sensor.error_message);

    // Print the indicators configuration
    ESP_LOGI("AQI_CONFIG", "Applying Indicators Configuration:");
    for (int i = 0; i < AQI_CONFIG_INDICATOR_NUM; i++)
    {
        ESP_LOGI("AQI_CONFIG", "    Indicator %d:", i + 1);
        ESP_LOGI("AQI_CONFIG", "        Threshold Warning: %d", config->indicator[i].threshold_warning);
        ESP_LOGI("AQI_CONFIG", "        Threshold Bad: %d", config->indicator[i].threshold_bad);
        ESP_LOGI("AQI_CONFIG", "        Color Good: %6X", config->indicator[i].color_good);
        ESP_LOGI("AQI_CONFIG", "        Color Warning: %6X", config->indicator[i].color_warning);
        ESP_LOGI("AQI_CONFIG", "        Color Bad: %6X", config->indicator[i].color_bad);
        ESP_LOGI("AQI_CONFIG", "        Name: %s", config->indicator[i].name);
        ESP_LOGI("AQI_CONFIG", "        Unit: %s", config->indicator[i].unit);
    }
}

int __aqi_config_spiffs_format()
{
    esp_err_t ret = esp_spiffs_format(NULL);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "SPIFFS formatted successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to format SPIFFS: %s", esp_err_to_name(ret));
    }

    return ret;
}

static int __aqi_config_spiffs_mount()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = AQI_CONFIG_SPIFFS_BASE_PATH,
        .partition_label = NULL,
        .max_files = AQI_CONFIG_SPIFFS_MAX_FILES,
        .format_if_mount_failed = true
    };

    // __aqi_config_spiffs_format();

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "SPIFFS mounted successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
    }

    return ret;
}

int aqi_config_init()
{
    aqi_config_t loaded_config = {0};

    __aqi_config_spiffs_mount();

    if (__aqi_config_load_from_spiffs(&loaded_config) != ESP_OK)
    {
        __aqi_config_save_to_spiffs(&_aqi_config);
    }
    else
    {
        _aqi_config = loaded_config;
    }

    __aqi_config_show_debug(&_aqi_config);

    return ESP_OK;
}

aqi_config_theme_t* aqi_config_theme_get()
{
    return &_aqi_config.theme;
}

aqi_config_sensor_t* aqi_config_sensor_get()
{
    return &_aqi_config.sensor;
}

aqi_config_indicator_t* aqi_config_indicator_get(uint8_t index)
{
    if (index < AQI_CONFIG_INDICATOR_NUM)
    {
        return &_aqi_config.indicator[index];
    }
    else
    {
        ESP_LOGE(TAG, "Index out of bounds");
        return NULL;
    }
}
