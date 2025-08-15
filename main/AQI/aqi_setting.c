
#include "aqi_setting.h"

#include "aqi_factory.h"

#include "aqi_selector.h"

#include "aqi_language.h"
#include "aqi_country.h"
#include "aqi_wifi.h"

#include "ui.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hsm.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_SETTING_CONTAINER_STATUS_SIZE 800, 47

#define AQI_SETTING_BUTTON_SELECT_LABEL_SELECT _("button_select")
#define AQI_SETTING_BUTTON_SELECT_LABEL_BACK   _("button_back")

typedef enum {
    AQI_SETTING_EVENT_SCREEN_LOADED = 1,
    AQI_SETTING_EVENT_SETTING,
    AQI_SETTING_EVENT_BACK_CLICKED,
    AQI_SETTING_EVENT_DARK_MODE_CLICKED,
    AQI_SETTING_EVENT_SOUND_ENABLE_CLICKED,
    AQI_SETTING_EVENT_SOUND_TAB_CLICKED,
    AQI_SETTING_EVENT_WIFI_ENABLE_CLICKED,
    AQI_SETTING_EVENT_WIFI_TAB_CLICKED,
    AQI_SETTING_EVENT_HISTORY_ENABLE_CLICKED,
    AQI_SETTING_EVENT_HISTORY_TAB_CLICKED,
    AQI_SETTING_EVENT_LANGUAGE_TAB_CLICKED,
    AQI_SETTING_EVENT_COUNTRY_TAB_CLICKED,
    AQI_SETTING_EVENT_BLUETOOTH_ENABLE_CLICKED,
    AQI_SETTING_EVENT_BLUETOOTH_TAB_CLICKED,
    AQI_SETTING_EVENT_EMAIL_ALERT_ENABLE_CLICKED,
    AQI_SETTING_EVENT_EMAIL_ALERT_TAB_CLICKED,
    AQI_SETTING_EVENT_WEB_EDIT_CLICKED,
    AQI_SETTING_EVENT_WEB_TAB_CLICKED,
    AQI_SETTING_EVENT_EMAIL_EDIT_CLICKED,
    AQI_SETTING_EVENT_EMAIL_TAB_CLICKED,
    AQI_SETTING_EVENT_SELECT_BUTTON_CLICKED,
} aqi_setting_event_t;

typedef enum {
    aqi_setting_state_idle,
    aqi_setting_state_tab,
    aqi_setting_state_language,
    aqi_setting_state_country,
    aqi_setting_state_wifi
} aqi_setting_state_t;

typedef struct {
    state_machine_t machine;  //!< Abstract state machine
} aqi_setting_t;

static const char *TAG = "AQI SETTING";

HSM_STATE_HANDLER_DECLARE(aqi_setting_state_idle);
HSM_STATE_HANDLER_DECLARE(aqi_setting_state_tab);
HSM_STATE_HANDLER_DECLARE(aqi_setting_state_language);
HSM_STATE_HANDLER_DECLARE(aqi_setting_state_country);
HSM_STATE_HANDLER_DECLARE(aqi_setting_state_wifi);

static const state_t _aqi_setting_states[] = {
    HSM_STATE_DECLARE(aqi_setting_state_idle),
    HSM_STATE_DECLARE(aqi_setting_state_tab),
    HSM_STATE_DECLARE(aqi_setting_state_language),
    HSM_STATE_DECLARE(aqi_setting_state_country),
    HSM_STATE_DECLARE(aqi_setting_state_wifi),
};

static aqi_setting_t _aqi_setting = {0};

/********************************** Static Function **********************************/
static void __aqi_setting_init()
{
    static uint8_t initialed = 0;

    if (initialed == 0)
    {
        initialed = 1;
        ESP_LOGI(TAG, "Initializing setting");

        aqi_wifi_init();
        aqi_country_init();        
        aqi_language_init();

        aqi_factory_init();
    }
}

static void __aqi_setting_event(aqi_setting_event_t event)
{
    _aqi_setting.machine.Event = event;
    dispatch_event((state_machine_t* const []){&_aqi_setting.machine}, 1);
}

static void __aqi_setting_event_handler(lv_event_t *event)
{
    aqi_setting_event_t aqi_setting_event = (uintptr_t)lv_event_get_user_data(event);

    ESP_LOGI(TAG, "Event triggered: %d", aqi_setting_event);

    __aqi_setting_event(aqi_setting_event);
}

/********************************** Public Function **********************************/
int aqi_setting_init()
{
    _aqi_setting.machine.State = &_aqi_setting_states[aqi_setting_state_idle];

    aqi_selector_init();

    lv_obj_add_event_cb(ui_ContainerStatusCenter, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SETTING);
    lv_obj_add_event_cb(ui_ImageSetting, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SETTING);
    lv_obj_add_event_cb(ui_LabelSetting, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SETTING);

    lv_obj_add_event_cb(ui_ScreenSettingAQI, &__aqi_setting_event_handler,
                        LV_EVENT_SCREEN_LOADED, (void*)AQI_SETTING_EVENT_SCREEN_LOADED);

    lv_obj_add_event_cb(ui_ImageSettingTabSwitchBack, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_BACK_CLICKED);
    lv_obj_add_event_cb(ui_SwitchSettingTabDarkMode, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_DARK_MODE_CLICKED);

    lv_obj_add_event_cb(ui_SwitchSettingTabSoundEnable, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SOUND_ENABLE_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabSound, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SOUND_TAB_CLICKED);

    lv_obj_add_event_cb(ui_ContainerSettingTabWifi, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_WIFI_TAB_CLICKED);

    lv_obj_add_event_cb(ui_SwitchSettingTabHistoryEnable, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_HISTORY_ENABLE_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabHistory, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_HISTORY_TAB_CLICKED);

    lv_obj_add_event_cb(ui_ContainerSettingTabLanguage, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_LANGUAGE_TAB_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabCountry, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_COUNTRY_TAB_CLICKED);

    lv_obj_add_event_cb(ui_SwitchSettingTabBluetoothEnable, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_BLUETOOTH_ENABLE_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabBluetooth, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_BLUETOOTH_TAB_CLICKED);

    lv_obj_add_event_cb(ui_SwitchSettingTabEmailAlertEnable, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_EMAIL_ALERT_ENABLE_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabEmailAlert, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_EMAIL_ALERT_TAB_CLICKED);
                    
    lv_obj_add_event_cb(ui_ImageSettingTabWebEdit, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_WEB_EDIT_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabWeb, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_WEB_TAB_CLICKED);

    lv_obj_add_event_cb(ui_ImageSettingTabEmailEdit, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_EMAIL_EDIT_CLICKED);
    lv_obj_add_event_cb(ui_ContainerSettingTabEmail, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_EMAIL_TAB_CLICKED);

    lv_obj_add_event_cb(ui_ButtonSettingSelect, &__aqi_setting_event_handler,
                        LV_EVENT_CLICKED, (void*)AQI_SETTING_EVENT_SELECT_BUTTON_CLICKED);

    return ESP_OK;
}

/********************************** Idle State **********************************/
static state_machine_result_t __aqi_setting_state_idle_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to idle state");

    lv_obj_add_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);

    lv_scr_load_anim(ui_ScreenIndicatorAQI, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_idle_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_SETTING_EVENT_SCREEN_LOADED:
        {
            ESP_LOGI(TAG, "Setting screen loaded event triggered");

            // Update the status container
            lv_obj_set_parent(ui_ContainerStatus, ui_ContainerSettingMain);
            lv_obj_set_size(ui_ContainerStatus, AQI_SETTING_CONTAINER_STATUS_SIZE);
            lv_obj_add_flag(ui_ContainerStatusCenter, LV_OBJ_FLAG_HIDDEN);

            __aqi_setting_init();
            break;
        }

        case AQI_SETTING_EVENT_SETTING:
        {
            ESP_LOGI(TAG, "Setting clicked event triggered");

            lv_scr_load_anim(ui_ScreenSettingAQI, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_tab]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_idle_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from idle state");

    lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Tab State **********************************/
static state_machine_result_t __aqi_setting_state_tab_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to tab state");

    lv_obj_clear_flag(ui_ContainerSettingTab, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ButtonSettingSelect, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_tab_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_SETTING_EVENT_BACK_CLICKED:
        {
            ESP_LOGI(TAG, "Back button clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_idle]);
        }

        case AQI_SETTING_EVENT_LANGUAGE_TAB_CLICKED:
        {
            ESP_LOGI(TAG, "Language tab clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_language]);
        }

        case AQI_SETTING_EVENT_COUNTRY_TAB_CLICKED:
        {
            ESP_LOGI(TAG, "Country tab clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_country]);
        }

        case AQI_SETTING_EVENT_WIFI_TAB_CLICKED:
        {
            ESP_LOGI(TAG, "Wifi tab clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_wifi]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_tab_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from tab state");

    lv_obj_add_flag(ui_ContainerSettingTab, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ButtonSettingSelect, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Language State **********************************/
static state_machine_result_t __aqi_setting_state_language_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to language state");

    lv_obj_clear_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_language_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_SETTING_EVENT_SELECT_BUTTON_CLICKED:
        {
            ESP_LOGI(TAG, "Select button clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_tab]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_language_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from language state");

    lv_obj_add_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Country State **********************************/
static state_machine_result_t __aqi_setting_state_country_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to country state");

    lv_obj_clear_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_country_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_SETTING_EVENT_SELECT_BUTTON_CLICKED:
        {
            ESP_LOGI(TAG, "Select button clicked event triggered");
            return switch_state(pState, &_aqi_setting_states[aqi_setting_state_tab]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_country_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from country state");

    lv_obj_add_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Wifi State **********************************/
static state_machine_result_t __aqi_setting_state_wifi_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to wifi state");

    lv_label_set_text(ui_LabelSettingSelectButton, AQI_SETTING_BUTTON_SELECT_LABEL_BACK);
    lv_obj_clear_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    aqi_wifi_event(aqi_wifi_event_config);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_wifi_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_SETTING_EVENT_SELECT_BUTTON_CLICKED:
        {
            ESP_LOGI(TAG, "Select button clicked event triggered");

            // if (aqi_wifi_exit_condition())
            {
                return switch_state(pState, &_aqi_setting_states[aqi_setting_state_tab]);
            }
            break;
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_setting_state_wifi_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from wifi state");

    lv_label_set_text(ui_LabelSettingSelectButton, AQI_SETTING_BUTTON_SELECT_LABEL_SELECT);
    lv_obj_add_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    aqi_wifi_event(aqi_wifi_event_setting_done);

    return EVENT_HANDLED;
}
