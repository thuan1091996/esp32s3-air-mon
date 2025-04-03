
#include "aqi_wifi.h"

#include "aqi_utility.h"

#include "ui.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_WIFI_SSID_MAX          20

#define AQI_WIFI_TASK_NAME         "AQI WiFi Task"
#define AQI_WIFI_TASK_STACK_SIZE   (10 * 1024)
#define AQI_WIFI_TASK_PRIORITY     5

#define AQI_WIFI_NTP_SERVER_NAME   "time.google.com"

#define AQI_WIFI_TITLE_NAME        "Wi-Fi Settings"

typedef enum {
    aqi_wifi_event_change_state   = BIT0,
    aqi_wifi_event_change_enable  = BIT1,
    aqi_wifi_event_all            = 0xFFFF
} aqi_wifi_event_t;

typedef enum {
    aqi_wifi_state_none,
    aqi_wifi_state_config,
    aqi_wifi_state_scan,
    aqi_wifi_state_scan_done,
    aqi_wifi_state_select,
    aqi_wifi_state_connect,
    aqi_wifi_state_connecting,
    aqi_wifi_state_connected,
    aqi_wifi_state_disconnected,
    aqi_wifi_state_time_sync
} aqi_wifi_state_t;

typedef struct {
    lv_obj_t* item;
    lv_obj_t* icon;
    lv_obj_t* name;
    lv_obj_t* lock;
    lv_obj_t* tick;
} aqi_wifi_ssid_ui_t;

typedef struct {
    char name[32];
    char password[64];
} aqi_wifi_ssid_info_t;

typedef struct {
    uint16_t number;
    lv_obj_t* container;
    aqi_wifi_ssid_ui_t ui[AQI_WIFI_SSID_MAX];
    aqi_wifi_ssid_ui_t* select;
    aqi_wifi_ssid_info_t info;
} aqi_wifi_ssid_t;

typedef struct {
    uint8_t enable;
    uint8_t factory_setting;
    uint8_t connected;
    aqi_wifi_state_t state;
    aqi_wifi_ssid_t ssid;
    esp_netif_t* sta_netif;
    TaskHandle_t task_handle;
    EventGroupHandle_t event_handle;
    lv_obj_t* screen_setting;
    lv_obj_t* screen_change;
} aqi_wifi_t;

static const char *TAG = "AQI WIFI";

static aqi_wifi_t _aqi_wifi = {0};

static lv_style_t _container_style;
static lv_style_t _ssid_item_style;
static lv_style_t _ssid_name_style;

extern bool example_lvgl_lock(int timeout_ms);
extern void example_lvgl_unlock(void);

static void __aqi_wifi_init_styles()
{
    // Style for SSID container
    lv_style_init(&_container_style);
    lv_style_set_bg_color(&_container_style, lv_color_hex(0x30323B));
    lv_style_set_bg_opa(&_container_style, 82);
    lv_style_set_pad_all(&_container_style, 12);
    lv_style_set_radius(&_container_style, 8);

    // Style for SSID item
    lv_style_init(&_ssid_item_style);
    lv_style_set_bg_color(&_ssid_item_style, lv_color_hex(0x1F2127));
    lv_style_set_bg_opa(&_ssid_item_style, 255);
    lv_style_set_pad_left(&_ssid_item_style, 20);
    lv_style_set_pad_right(&_ssid_item_style, 12);
    lv_style_set_pad_top(&_ssid_item_style, 10);
    lv_style_set_pad_bottom(&_ssid_item_style, 10);

    // Style for SSID name
    lv_style_init(&_ssid_name_style);
    lv_style_set_text_font(&_ssid_name_style, &ui_font_Poppins_Regular_18);
}

static lv_obj_t *__aqi_wifi_create_image(lv_obj_t *parent, const void *src, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *img = lv_img_create(parent);
    lv_img_set_src(img, src);
    lv_obj_align_to(img, parent, align, x_offset, y_offset);
    return img;
}

static lv_obj_t *__aqi_wifi_create_label(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_align_to(label, parent, align, x_offset, y_offset);
    lv_obj_add_style(label, &_ssid_name_style, 0);
    return label;
}

static void __aqi_wifi_ssid_select_item_update(aqi_wifi_ssid_ui_t *ssid_ui, uint8_t select)
{
    lv_obj_set_style_bg_color(ssid_ui->item, lv_color_hex(select ? 0x30323B : 0x1F2127), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_img_set_src(ssid_ui->lock, select ? &ui_img_images_wifi_lock_20_white_png : &ui_img_images_wifi_lock_20_grey_png);
    select ? lv_obj_clear_flag(ssid_ui->tick, LV_OBJ_FLAG_HIDDEN) : lv_obj_add_flag(ssid_ui->tick, LV_OBJ_FLAG_HIDDEN);
}

static void __aqi_wifi_ssid_select_event_handler(lv_event_t *event)
{
    aqi_wifi_ssid_ui_t* ssid_ui = lv_event_get_user_data(event);

    if (_aqi_wifi.ssid.select != ssid_ui)
    {
        if (_aqi_wifi.ssid.select != NULL)
            __aqi_wifi_ssid_select_item_update(_aqi_wifi.ssid.select, 0);

        __aqi_wifi_ssid_select_item_update(ssid_ui, 1);
        strncpy(_aqi_wifi.ssid.info.name, lv_label_get_text(ssid_ui->name), sizeof(_aqi_wifi.ssid.info.name));
        _aqi_wifi.ssid.select = ssid_ui;
    }
    else
    {
        __aqi_wifi_ssid_select_item_update(ssid_ui, 0);
        _aqi_wifi.ssid.info.name[0] = '\0';
        _aqi_wifi.ssid.select = NULL;
    }
}

static int __aqi_wifi_create_ssid_container()
{
    lv_obj_t *container = lv_obj_create(ui_ContainerSettingWifi);
    lv_obj_add_style(container, &_container_style, 0);
    lv_obj_set_size(container, 736, 160);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 84);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_set_style_bg_opa(container, 82, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    _aqi_wifi.ssid.container = container;

    return ESP_OK;
}

static int __aqi_wifi_create_ssid(uint16_t ap_number, wifi_ap_record_t ap_info[])
{
    if (ap_number == 0 || _aqi_wifi.ssid.number)
        return ESP_FAIL;

    _aqi_wifi.ssid.number = ap_number;
    __aqi_wifi_create_ssid_container();

    for (int i = 0; i < ap_number; i++)
    {
        lv_obj_t *ssid_item = lv_obj_create(_aqi_wifi.ssid.container);
        lv_obj_add_style(ssid_item, &_ssid_item_style, 0);
        lv_obj_set_size(ssid_item, 730, 48);
        lv_obj_align(ssid_item, LV_ALIGN_TOP_MID, 0, i * 48);
        lv_obj_clear_flag(ssid_item, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *ssid_icon = __aqi_wifi_create_image(ssid_item, &ui_img_images_wifi_24_white_png, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_t *ssid_name = __aqi_wifi_create_label(ssid_item, (char *)ap_info[i].ssid, LV_ALIGN_LEFT_MID, 32, 0);
        lv_obj_t *ssid_lock = __aqi_wifi_create_image(ssid_name, &ui_img_images_wifi_lock_20_grey_png, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
        lv_obj_t *ssid_tick = __aqi_wifi_create_image(ssid_item, &ui_img_images_tick_green_png, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_add_flag(ssid_tick, LV_OBJ_FLAG_HIDDEN);

        _aqi_wifi.ssid.ui[i] = (aqi_wifi_ssid_ui_t){ssid_item, ssid_icon, ssid_name, ssid_lock, ssid_tick};
        lv_obj_add_event_cb(ssid_item, &__aqi_wifi_ssid_select_event_handler, LV_EVENT_CLICKED, &_aqi_wifi.ssid.ui[i]);
    }

    return ESP_OK;
}

static int __aqi_wifi_delete_ssid()
{
    if (_aqi_wifi.ssid.number == 0)
        return ESP_FAIL;

    lv_obj_del(_aqi_wifi.ssid.container);

    _aqi_wifi.ssid.container = NULL;
    _aqi_wifi.ssid.select = NULL;
    _aqi_wifi.ssid.number = 0;

    return ESP_OK;
}

static int __aqi_wifi_change_screen(aqi_wifi_t *aqi_wifi)
{
    if (lv_scr_act() == aqi_wifi->screen_setting)
    {
        lv_scr_load_anim(aqi_wifi->screen_change, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    }

    return ESP_OK;
}

static int __aqi_wifi_set_event(aqi_wifi_event_t event)
{
    return xEventGroupSetBits(_aqi_wifi.event_handle, event);
}

static int __aqi_wifi_set_state(aqi_wifi_state_t state)
{
    if (_aqi_wifi.state != state)
    {
        _aqi_wifi.state = state;
        return __aqi_wifi_set_event(aqi_wifi_event_change_state);
    }

    return ESP_OK;
}

static int __aqi_wifi_state_change_handler(aqi_wifi_t *aqi_wifi)
{
    switch (aqi_wifi->state)
    {
        case aqi_wifi_state_config:
        {
            ESP_LOGD(TAG, "Configuring factory settings...");
            __aqi_wifi_delete_ssid();
        }
        break;
        case aqi_wifi_state_scan:
        {
            ESP_LOGD(TAG, "Scanning for Wi-Fi networks...");
            lv_obj_clear_flag(ui_SwitchSettingWifiEnable, LV_OBJ_FLAG_CLICKABLE);
            ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));
        }
        break;
        case aqi_wifi_state_scan_done:
        {
            ESP_LOGD(TAG, "Scan completed.");
            uint16_t ap_number = AQI_WIFI_SSID_MAX;
            wifi_ap_record_t ap_info[AQI_WIFI_SSID_MAX];
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_number));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_number, ap_info));

            __aqi_wifi_create_ssid(ap_number, ap_info);
            lv_obj_add_flag(ui_SwitchSettingWifiEnable, LV_OBJ_FLAG_CLICKABLE);
            aqi_wifi->state = aqi_wifi_state_select;
        }
        break;
        case aqi_wifi_state_select:
        {
            lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_ContainerWifiPassword, LV_OBJ_FLAG_HIDDEN);
        }
        break;
        case aqi_wifi_state_connect:
        {
            ESP_LOGD(TAG, "Enter password for Wi-Fi network: %s", aqi_wifi->ssid.info.name);
            lv_label_set_text(ui_LabelWifiSelectName, aqi_wifi->ssid.info.name);
            lv_textarea_set_text(ui_TextWifiPasswordType, "");
            lv_keyboard_set_mode(ui_KeyboardWifiPassword, LV_KEYBOARD_MODE_TEXT_LOWER);
            lv_obj_add_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_ContainerWifiPassword, LV_OBJ_FLAG_HIDDEN);
        }
        break;
        case aqi_wifi_state_connecting:
        {
            ESP_LOGD(TAG, "Connecting to Wi-Fi network: %s", aqi_wifi->ssid.info.name);
            wifi_config_t wifi_config = {.sta = {.threshold.authmode = WIFI_AUTH_WPA2_PSK}};
            strncpy((char *)wifi_config.sta.ssid, (char *)aqi_wifi->ssid.info.name, sizeof(wifi_config.sta.ssid));
            strncpy((char *)wifi_config.sta.password, aqi_wifi->ssid.info.password, sizeof(wifi_config.sta.password));
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            if (aqi_wifi->connected)
            {
                ESP_ERROR_CHECK(esp_wifi_disconnect());
            }
            else
            {
                ESP_ERROR_CHECK(esp_wifi_connect());
            }
        }
        break;
        case aqi_wifi_state_connected:
        {
            aqi_wifi->connected = 1;
            ESP_ERROR_CHECK(esp_netif_dhcpc_start(aqi_wifi->sta_netif));
            // TODO: save ssid and password to NVS
            __aqi_wifi_delete_ssid();
            __aqi_wifi_change_screen(aqi_wifi);
        }
        break;
        case aqi_wifi_state_disconnected:
        {
            if (aqi_wifi->connected)
            {
                aqi_wifi->connected = 0;
                ESP_ERROR_CHECK(esp_netif_dhcpc_stop(aqi_wifi->sta_netif));
            }
            ESP_ERROR_CHECK(esp_wifi_connect());
        }
        default:
            break;
    }

    return ESP_OK;
}

static void __aqi_wifi_task_handler(void *pvParameters)
{
    aqi_wifi_t *aqi_wifi = (aqi_wifi_t *)pvParameters;

    while (true)
    {
        EventBits_t events = xEventGroupWaitBits(aqi_wifi->event_handle, aqi_wifi_event_all, pdTRUE, pdFALSE, portMAX_DELAY);

        example_lvgl_lock(-1);

        if (events & aqi_wifi_event_change_state)
        {
            __aqi_wifi_state_change_handler(aqi_wifi);
        }

        if (events & aqi_wifi_event_change_enable)
        {
            ESP_LOGD(TAG, "Wi-Fi enable state changed: %d", aqi_wifi->enable);
            __aqi_wifi_set_state(aqi_wifi->enable ? aqi_wifi_state_scan : aqi_wifi_state_config);
        }

        example_lvgl_unlock();
    }

    vTaskDelete(NULL);
}

static void __ui_event_handler(lv_event_t *event)
{
    lv_obj_t *target = lv_event_get_target(event);
    aqi_wifi_t *aqi_wifi = lv_event_get_user_data(event);

    if (target == ui_SwitchSettingWifiEnable)
    {
        aqi_wifi->enable = lv_obj_has_state(ui_SwitchSettingWifiEnable, LV_STATE_CHECKED);
        __aqi_wifi_set_event(aqi_wifi_event_change_enable);
    }
    else if (target == ui_ButtonSettingNext)
    {
        if (aqi_wifi->enable)
        {
            if (aqi_wifi->state == aqi_wifi_state_select && aqi_wifi->ssid.select != NULL)
            {
                __aqi_wifi_set_state(aqi_wifi_state_connect);
            }
        }
        else
        {
            __aqi_wifi_change_screen(aqi_wifi);
        }
    }
    else if (target == ui_ButtonWifiConnect)
    {
        strncpy(aqi_wifi->ssid.info.password, lv_textarea_get_text(ui_TextWifiPasswordType), sizeof(aqi_wifi->ssid.info.password));
        if (strlen(aqi_wifi->ssid.info.password))
        {
            __aqi_wifi_set_state(aqi_wifi_state_connecting);
        }
        else
        {
            ESP_LOGE(TAG, "Password is empty");
        }        
    }
    else if (target == ui_ImageWifiSelectExit)
    {
        __aqi_wifi_set_state(aqi_wifi_state_select);
    }
}

// Event handler function
static void __wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                ESP_LOGD(TAG, "Wi-Fi started.");
                // esp_wifi_connect();
                break;
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGD(TAG, "Wi-Fi scan done.");
                __aqi_wifi_set_state(aqi_wifi_state_scan_done);
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGD(TAG, "Wi-Fi connected.");
                __aqi_wifi_set_state(aqi_wifi_state_connected);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "Wi-Fi disconnected, retrying...");
                __aqi_wifi_set_state(aqi_wifi_state_disconnected);
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
    __aqi_wifi_set_state(aqi_wifi_state_time_sync);
}

int aqi_wifi_init()
{
    // Initialize Event Group and Task
    _aqi_wifi.event_handle = xEventGroupCreate();
    xTaskCreate(__aqi_wifi_task_handler, AQI_WIFI_TASK_NAME, AQI_WIFI_TASK_STACK_SIZE, &_aqi_wifi, AQI_WIFI_TASK_PRIORITY, NULL);

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
    sntp_set_time_sync_notification_cb(__aqi_wifi_time_sync_callback);
    esp_sntp_init();

    // Initialize UI elements
    _aqi_wifi.screen_setting = ui_ScreenSettingAQI;
    _aqi_wifi.screen_change = ui_ScreenIndicatorAQI;
    __aqi_wifi_init_styles();
    lv_obj_add_event_cb(ui_SwitchSettingWifiEnable, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_ButtonWifiConnect, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_ImageWifiSelectExit, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);
    lv_obj_add_event_cb(ui_ButtonSettingNext, &__ui_event_handler, LV_EVENT_CLICKED, &_aqi_wifi);

    return ESP_OK;
}

int aqi_wifi_config_factory()
{
    _aqi_wifi.factory_setting = 1;
    lv_img_set_src(ui_ImageSettingStageWifi, &ui_img_images_stage_green_png);
    lv_img_set_src(ui_ImageSettingTitleIcon, &ui_img_images_wifi_44_png);
    lv_label_set_text(ui_LabelSettingTitleName, AQI_WIFI_TITLE_NAME);
    lv_obj_clear_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    lv_event_send(ui_SwitchSettingWifiEnable, LV_EVENT_CLICKED, NULL);
    return ESP_OK;
}
