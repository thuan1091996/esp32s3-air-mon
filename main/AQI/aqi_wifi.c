
#include "aqi_wifi.h"

#include "aqi_utility.h"

#include "aqi_setting.h"
#include "aqi_factory.h"

#include "aqi_selector.h"

#include "ui.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"

#include "hsm.h"

#include <time.h>

#include "esp_err.h"
#include "esp_log.h"

#define AQI_WIFI_SSID_MAX          20
#define AQI_WIFI_SSID_NAME_MAX     32
#define AQI_WIFI_SSID_PASSWORD_MAX 64

#define AQI_WIFI_TASK_NAME       "AQI WiFi Task"
#define AQI_WIFI_TASK_STACK_SIZE (10 * 1024)
#define AQI_WIFI_TASK_PRIORITY   5

#define AQI_WIFI_TIMER_NAME        "AQI Wifi Timer"
#define AQI_WIFI_TIMER_INTERVAL_MS 10000

// Signal strength thresholds (in dBm)
#define AQI_WIFI_SIGNAL_EXCELLENT -50
#define AQI_WIFI_SIGNAL_GOOD      -70

#define AQI_WIFI_NTP_SERVER_NAME   "time.google.com"

#define AQI_WIFI_SSID_CONTAINER_SIZE   736, 160
#define AQI_WIFI_SSID_CONTAINER_OFFSET 0, 152

#define AQI_WIFI_SSID_ITEM_SIZE      730, 48
#define AQI_WIFI_SSID_ITEM_OFFSET(i) 0, i * 48

#define AQI_WIFI_SSID_ICON_OFFSET 0, 0
#define AQI_WIFI_SSID_NAME_OFFSET 32, 0
#define AQI_WIFI_SSID_LOCK_OFFSET 8, 0
#define AQI_WIFI_SSID_TICK_OFFSET 0, 0

#define AQI_WIFI_STATE_HANDLER_DECLARE(state) \
    static state_machine_result_t __##state##_handler(state_machine_t* const State); \
    static state_machine_result_t __##state##_entry_handler(state_machine_t* const State); \
    static state_machine_result_t __##state##_exit_handler(state_machine_t* const State);

#define AQI_WIFI_STATE_ADD(state) \
    [aqi_wifi_state_##state] = {\
        .Handler = __##state##_handler, \
        .Entry   = __##state##_entry_handler, \
        .Exit    = __##state##_exit_handler,  \
    }

typedef enum {
    aqi_wifi_state_idle,
    aqi_wifi_state_config,
    aqi_wifi_state_scan,
    aqi_wifi_state_connect
} aqi_wifi_state_t;

typedef struct {
    aqi_selector_t *selector;
    lv_obj_t* lock;
} aqi_wifi_ssid_ui_t;

typedef struct {
    char name[AQI_WIFI_SSID_NAME_MAX];
    char password[AQI_WIFI_SSID_PASSWORD_MAX];
} aqi_wifi_ssid_info_t;

typedef struct {
    uint16_t number;
    lv_obj_t* container;
    aqi_wifi_ssid_ui_t ui[AQI_WIFI_SSID_MAX];
    aqi_wifi_ssid_ui_t* select;
    aqi_wifi_ssid_info_t info;
} aqi_wifi_ssid_t;

typedef struct {
    state_machine_t machine;  //!< Abstract state machine

    uint8_t enable;
    uint8_t connected;

    aqi_wifi_state_t state;

    aqi_wifi_ssid_t ssid;
    esp_netif_t* sta_netif;

    TaskHandle_t task_handle;
    EventGroupHandle_t event_handle;

    lv_timer_t* timer_handle;
    int8_t last_rssi;
} aqi_wifi_t;

extern bool example_lvgl_lock(int timeout_ms);
extern void example_lvgl_unlock(void);

static const char *TAG = "AQI WIFI";

AQI_WIFI_STATE_HANDLER_DECLARE(idle);
AQI_WIFI_STATE_HANDLER_DECLARE(config);
AQI_WIFI_STATE_HANDLER_DECLARE(scan);
AQI_WIFI_STATE_HANDLER_DECLARE(connect);

static const state_t _aqi_wifi_states[] = {
    AQI_WIFI_STATE_ADD(idle),
    AQI_WIFI_STATE_ADD(config),
    AQI_WIFI_STATE_ADD(scan),
    AQI_WIFI_STATE_ADD(connect),
};

static aqi_wifi_t _aqi_wifi = {0};

static void __aqi_wifi_ssid_select_item_update(aqi_wifi_ssid_ui_t *ssid_ui, uint8_t select)
{
    if (select)
    {
        lv_img_set_src(ssid_ui->lock, &ui_img_images_wifi_lock_select_png);
        lv_obj_clear_flag(ssid_ui->selector->tick, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_bg_color(ssid_ui->selector->item, AQI_SETTING_ITEM_BG_COLOR_SELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
    {
        lv_img_set_src(ssid_ui->lock, &ui_img_images_wifi_lock_unselect_png);
        lv_obj_add_flag(ssid_ui->selector->tick, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_bg_color(ssid_ui->selector->item, AQI_SETTING_ITEM_BG_COLOR_UNSELECT, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void __aqi_wifi_ssid_select_event_handler(lv_event_t *event)
{
    aqi_selector_t* selector = lv_event_get_user_data(event);

    if (_aqi_wifi.ssid.select != NULL)
    {
        __aqi_wifi_ssid_select_item_update(_aqi_wifi.ssid.select, 0);
    }

    _aqi_wifi.ssid.select = &_aqi_wifi.ssid.ui[selector->index];
    __aqi_wifi_ssid_select_item_update(_aqi_wifi.ssid.select, 1);

    strncpy(_aqi_wifi.ssid.info.name, lv_label_get_text(_aqi_wifi.ssid.select->selector->name), sizeof(_aqi_wifi.ssid.info.name));
    aqi_wifi_event(aqi_wifi_event_select);
}

static int __aqi_wifi_create_ssid(uint16_t ap_number, wifi_ap_record_t ap_info[])
{
    if (ap_number == 0 || _aqi_wifi.ssid.number)
        return ESP_FAIL;

    _aqi_wifi.ssid.number = ap_number;
    _aqi_wifi.ssid.container = aqi_setting_create_container(ui_ContainerSettingWifi,
                                                            AQI_WIFI_SSID_CONTAINER_SIZE, AQI_WIFI_SSID_CONTAINER_OFFSET);

    for (int i = 0; i < ap_number; i++)
    {
        _aqi_wifi.ssid.ui[i].selector = aqi_selector_create(i, (char *)ap_info[i].ssid,
                                                            _aqi_wifi.ssid.container, &ui_img_images_wifi_white_24_png,
                                                            __aqi_wifi_ssid_select_event_handler);

        _aqi_wifi.ssid.ui[i].lock = aqi_setting_create_image(_aqi_wifi.ssid.ui[i].selector->name,
                                                             &ui_img_images_wifi_lock_unselect_png,
                                                             LV_ALIGN_OUT_RIGHT_MID, AQI_WIFI_SSID_LOCK_OFFSET);
    }

    return ESP_OK;
}

static int __aqi_wifi_delete_ssid(aqi_wifi_t* aqi_wifi)
{
    if (aqi_wifi->ssid.number == 0)
        return ESP_FAIL;

    if (aqi_wifi->connected)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
    }

    lv_obj_del(aqi_wifi->ssid.container);

    aqi_wifi->ssid.container = NULL;
    aqi_wifi->ssid.select = NULL;
    aqi_wifi->ssid.number = 0;

    return ESP_OK;
}

static void __aqi_wifi_task_handler(void *pvParameters)
{
    aqi_wifi_t *aqi_wifi = (aqi_wifi_t *)pvParameters;

    while (true)
    {
        EventBits_t events = xEventGroupWaitBits(aqi_wifi->event_handle, aqi_wifi_event_all, pdTRUE, pdFALSE, portMAX_DELAY);

        example_lvgl_lock(-1);

        aqi_wifi->machine.Event = events;
        dispatch_event((state_machine_t* const []){&aqi_wifi->machine}, 1);

        example_lvgl_unlock();
    }

    vTaskDelete(NULL);
}

static void __aqi_wifi_timer_handler(lv_timer_t *timer)
{
    wifi_ap_record_t ap_info;

    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK)
        return;

    // Only update UI if RSSI has changed significantly (to reduce UI updates)
    if (abs(ap_info.rssi - _aqi_wifi.last_rssi) >= 3 || _aqi_wifi.last_rssi == 0)
    {
        ESP_LOGD(TAG, "WiFi signal strength: %d dBm", ap_info.rssi);

        _aqi_wifi.last_rssi = ap_info.rssi;

        if (ap_info.rssi >= AQI_WIFI_SIGNAL_EXCELLENT)
            lv_img_set_src(ui_ImageWifiSignal, &ui_img_images_wifi_signal_excellent_png);
        else if (ap_info.rssi >= AQI_WIFI_SIGNAL_GOOD)
            lv_img_set_src(ui_ImageWifiSignal, &ui_img_images_wifi_signal_good_png);
        else
            lv_img_set_src(ui_ImageWifiSignal, &ui_img_images_wifi_signal_poor_png);
    }
}

static void __ui_event_handler(lv_event_t *event)
{
    lv_obj_t *target = lv_event_get_target(event);
    aqi_wifi_t *aqi_wifi = lv_event_get_user_data(event);

    if (target == ui_SwitchSettingWifiEnable)
    {
        ESP_LOGI(TAG, "Wi-Fi enable switch toggled");
        aqi_wifi_event(aqi_wifi_event_config);
    }
    else if (target == ui_LabelWifiPasswordShow)
    {
        bool is_checked = lv_obj_has_state(ui_LabelWifiPasswordShow, LV_STATE_CHECKED);
        lv_label_set_text(ui_LabelWifiPasswordShow, is_checked ? LV_SYMBOL_EYE_CLOSE : LV_SYMBOL_EYE_OPEN);
        lv_textarea_set_password_mode(ui_TextWifiPasswordType, is_checked);
    }
    else if (target == ui_ButtonWifiConnect)
    {
        strncpy(aqi_wifi->ssid.info.password, lv_textarea_get_text(ui_TextWifiPasswordType), sizeof(aqi_wifi->ssid.info.password));
        if (strlen(aqi_wifi->ssid.info.password))
        {
            aqi_wifi_event(aqi_wifi_event_connect);
        }    
    }
    else if (target == ui_ImageWifiSelectExit)
    {
        lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_ContainerWifiPassword, LV_OBJ_FLAG_HIDDEN);
    }
}

static void __wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                ESP_LOGD(TAG, "Wi-Fi started");
                break;
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGD(TAG, "Wi-Fi scan done");
                aqi_wifi_event(aqi_wifi_event_scan_done);
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGD(TAG, "Wi-Fi connected");
                aqi_wifi_event(aqi_wifi_event_connected);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "Wi-Fi disconnected");
                aqi_wifi_event(aqi_wifi_event_disconnected);
                break;
            default:
                ESP_LOGD(TAG, "Unhandled Wi-Fi event: %d", event_id);
                break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                esp_netif_ip_info_t *ip_info = (esp_netif_ip_info_t *)event_data;
                ESP_LOGD(TAG, "IP acquired: " IPSTR, IP2STR(&ip_info->ip));
                break;
            default:
                ESP_LOGD(TAG, "Unhandled IP event: %d", event_id);
                break;
        }
    }
}

void __aqi_wifi_time_sync_callback(struct timeval *tv)
{
    time_t now = 0;
    struct tm timeinfo = { 0 };

    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_LOGI(TAG, "Current time synchronized: %s", asctime(&timeinfo));

    aqi_utility_update_time(timeinfo);
}

void __aqi_wifi_connect(aqi_wifi_t *aqi_wifi)
{
    ESP_LOGD(TAG, "Connecting to Wi-Fi network: %s", aqi_wifi->ssid.info.name);

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, aqi_wifi->ssid.info.name, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, aqi_wifi->ssid.info.password, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void __aqi_wifi_connected(aqi_wifi_t *aqi_wifi)
{
    ESP_LOGD(TAG, "Connected to Wi-Fi network: %s", aqi_wifi->ssid.info.name);

    aqi_wifi->connected = 1;
    ESP_ERROR_CHECK(esp_netif_dhcpc_start(aqi_wifi->sta_netif));
    sntp_restart();

    aqi_wifi->last_rssi = 0; // Reset RSSI toz force update
    lv_obj_clear_flag(ui_ImageWifiSignal, LV_OBJ_FLAG_HIDDEN);
    lv_timer_resume(_aqi_wifi.timer_handle);
}

void __aqi_wifi_disconnected(aqi_wifi_t *aqi_wifi)
{
    if (aqi_wifi->connected == 0)
        return;

    ESP_LOGW(TAG, "Disconnected from Wi-Fi network: %s", aqi_wifi->ssid.info.name);

    aqi_wifi->connected = 0;
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(aqi_wifi->sta_netif));

    lv_obj_add_flag(ui_ImageWifiSignal, LV_OBJ_FLAG_HIDDEN);
    lv_timer_pause(_aqi_wifi.timer_handle);
}

int aqi_wifi_init()
{
    _aqi_wifi.machine.State = &_aqi_wifi_states[aqi_wifi_state_idle];

    // Initialize Event Group and Task
    _aqi_wifi.event_handle = xEventGroupCreate();
    xTaskCreate(__aqi_wifi_task_handler, AQI_WIFI_TASK_NAME,
                AQI_WIFI_TASK_STACK_SIZE, &_aqi_wifi, AQI_WIFI_TASK_PRIORITY, NULL);

    // Initialize Timer
    _aqi_wifi.timer_handle = lv_timer_create(__aqi_wifi_timer_handler, AQI_WIFI_TIMER_INTERVAL_MS, NULL);
    lv_timer_pause(_aqi_wifi.timer_handle);

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &__wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &__wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_netif_create_default_wifi_mesh_netifs(&_aqi_wifi.sta_netif, NULL));
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, AQI_WIFI_NTP_SERVER_NAME);
    sntp_set_time_sync_notification_cb((void*)__aqi_wifi_time_sync_callback);
    esp_sntp_init();

    // Initialize UI elements
    lv_obj_add_event_cb(ui_SwitchSettingWifiEnable, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_ButtonWifiConnect, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_LabelWifiPasswordShow, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_ImageWifiSelectExit, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);

    return ESP_OK;
}

void aqi_wifi_event(aqi_wifi_event_t event)
{
    xEventGroupSetBits(_aqi_wifi.event_handle, event);
}

/********************************** Idle State **********************************/
static state_machine_result_t __idle_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to idle state");

    lv_obj_add_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __idle_handler(state_machine_t* const pState)
{
    aqi_wifi_t* const aqi_wifi = (aqi_wifi_t*)pState;

    switch (pState->Event)
    {
        case aqi_wifi_event_config:
        {
            switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_config]);
            return TRIGGERED_TO_SELF;
        }

        case aqi_wifi_event_connected:
        {
            __aqi_wifi_connected(aqi_wifi);
            break;
        }

        case aqi_wifi_event_disconnected:
        {
            __aqi_wifi_disconnected(aqi_wifi);

            ESP_LOGW(TAG, "Retrying connection to Wi-Fi network: %s", aqi_wifi->ssid.info.name);

            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __idle_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from idle state");

    lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Config State **********************************/
static state_machine_result_t __config_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to config state");

    aqi_factory_button_clickable(true);

    return EVENT_HANDLED;
}

static state_machine_result_t __config_handler(state_machine_t* const pState)
{
    aqi_wifi_t* const aqi_wifi = (aqi_wifi_t*)pState;

    switch (pState->Event)
    {
        case aqi_wifi_event_config:
        {
            if (lv_obj_has_state(ui_SwitchSettingWifiEnable, LV_STATE_CHECKED))
            {
                return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_scan]);
            }
            else
            {
                __aqi_wifi_delete_ssid(aqi_wifi);
                aqi_factory_button_clickable(true);
                break;
            }
        }

        case aqi_wifi_event_select:
        {
            ESP_LOGD(TAG, "Enter password for Wi-Fi network: %s", aqi_wifi->ssid.info.name);

            lv_label_set_text(ui_LabelWifiSelectName, aqi_wifi->ssid.info.name);
            lv_label_set_text(ui_LabelWifiPasswordShow, LV_SYMBOL_EYE_CLOSE);

            lv_textarea_set_password_mode(ui_TextWifiPasswordType, true);
            lv_textarea_set_text(ui_TextWifiPasswordType, "");
            lv_keyboard_set_mode(ui_KeyboardWifiPassword, LV_KEYBOARD_MODE_TEXT_LOWER);
            
            lv_obj_add_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_ContainerWifiPassword, LV_OBJ_FLAG_HIDDEN);
            break;
        }

        case aqi_wifi_event_connect:
        {
            if (aqi_wifi->connected)
            {
                ESP_ERROR_CHECK(esp_wifi_disconnect());
            }
            __aqi_wifi_connect(aqi_wifi);

            return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_connect]);
        }

        case aqi_wifi_event_connected:
        {
            __aqi_wifi_connected(aqi_wifi);
            break;
        }

        case aqi_wifi_event_disconnected:
        {
            __aqi_wifi_disconnected(aqi_wifi);
            break;
        }

        case aqi_wifi_event_setting_done:
        {
            ESP_LOGI(TAG, "Finishing factory configuration");
            return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_idle]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __config_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from config state");

    aqi_factory_button_clickable(false);

    return EVENT_HANDLED;
}

/********************************** Scan State **********************************/
static state_machine_result_t __scan_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to scan state");

    lv_obj_clear_flag(ui_SwitchSettingWifiEnable, LV_OBJ_FLAG_CLICKABLE);

    ESP_LOGI(TAG, "Starting Wi-Fi scan");
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));

    return EVENT_HANDLED;
}

static state_machine_result_t __scan_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case aqi_wifi_event_scan_done:
        {
            ESP_LOGD(TAG, "Scan completed");

            uint16_t ap_number = AQI_WIFI_SSID_MAX;
            wifi_ap_record_t ap_info[AQI_WIFI_SSID_MAX];
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_number));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_number, ap_info));

            __aqi_wifi_create_ssid(ap_number, ap_info);
            return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_config]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __scan_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from scan state");

    lv_obj_add_flag(ui_SwitchSettingWifiEnable, LV_OBJ_FLAG_CLICKABLE);

    return EVENT_HANDLED;
}

/********************************** Connect State **********************************/
static state_machine_result_t __connect_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to connect state");

    lv_obj_clear_flag(ui_ButtonWifiConnect, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_ImageWifiSelectExit, LV_OBJ_FLAG_CLICKABLE);

    return EVENT_HANDLED;
}

static state_machine_result_t __connect_handler(state_machine_t* const pState)
{
    aqi_wifi_t* const aqi_wifi = (aqi_wifi_t*)pState;

    switch (pState->Event)
    {
        case aqi_wifi_event_connected:
        {
            __aqi_wifi_connected(aqi_wifi);

            lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_ContainerWifiPassword, LV_OBJ_FLAG_HIDDEN);

            // TODO: save ssid and password to NVS
            return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_config]);
        }

        case aqi_wifi_event_disconnected:
        {
            if (aqi_wifi->connected)
            {
                __aqi_wifi_disconnected(aqi_wifi);
                break;
            }
            else
            {
                return switch_state(pState, &_aqi_wifi_states[aqi_wifi_state_config]);
            }
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __connect_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from connect state");

    lv_obj_add_flag(ui_ButtonWifiConnect, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui_ImageWifiSelectExit, LV_OBJ_FLAG_CLICKABLE);

    return EVENT_HANDLED;
}
