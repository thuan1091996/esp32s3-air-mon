
#include "aqi_factory.h"

#include "aqi_language.h"
#include "aqi_country.h"
#include "aqi_wifi.h"

#include "ui.h"

#include "hsm.h"

#include "esp_err.h"
#include "esp_log.h"

typedef enum {
    AQI_FACTORY_EVENT_BACK = 1,
    AQI_FACTORY_EVENT_NEXT,
} aqi_factory_event_t;

typedef enum {
    aqi_factory_state_language,
    aqi_factory_state_country,
    aqi_factory_state_wifi,
    aqi_factory_state_done
} aqi_factory_state_t;

typedef struct {
    state_machine_t machine;  //!< Abstract state machine

    uint8_t setting_done;
} aqi_factory_t;

static const char *TAG = "AQI FACTORY";

HSM_STATE_HANDLER_DECLARE(aqi_factory_state_language);
HSM_STATE_HANDLER_DECLARE(aqi_factory_state_country);
HSM_STATE_HANDLER_DECLARE(aqi_factory_state_wifi);
HSM_STATE_HANDLER_DECLARE(aqi_factory_state_done);

static const state_t _aqi_factory_states[] = {
    HSM_STATE_DECLARE(aqi_factory_state_language),
    HSM_STATE_DECLARE(aqi_factory_state_country),
    HSM_STATE_DECLARE(aqi_factory_state_wifi),
    HSM_STATE_DECLARE(aqi_factory_state_done),
};

static aqi_factory_t _aqi_factory = {0};

/********************************** Static Function **********************************/
static void __aqi_factory_event(aqi_factory_event_t event)
{
    _aqi_factory.machine.Event = event;
    dispatch_event((state_machine_t* const []){&_aqi_factory.machine}, 1);
}

static void __aqi_factory_event_handler(lv_event_t *event)
{
    lv_obj_t *target = lv_event_get_target(event);

    if (target == ui_ButtonSettingBack)
    {
        ESP_LOGI(TAG, "Back button clicked");
        __aqi_factory_event(AQI_FACTORY_EVENT_BACK);
    }
    else if (target == ui_ButtonSettingNext)
    {
        ESP_LOGI(TAG, "Next button clicked");
        __aqi_factory_event(AQI_FACTORY_EVENT_NEXT);
    }
}

/********************************** Public Function **********************************/
void aqi_factory_init()
{
    if (_aqi_factory.setting_done != 0)
        return;

    // Add event handlers for buttons
    lv_obj_add_event_cb(ui_ButtonSettingBack, &__aqi_factory_event_handler, LV_EVENT_CLICKED, &_aqi_factory);
    lv_obj_add_event_cb(ui_ButtonSettingNext, &__aqi_factory_event_handler, LV_EVENT_CLICKED, &_aqi_factory);

    // Show the main container, buttons and stage 
    lv_obj_clear_flag(ui_ContainerSettingButton, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);
    lv_img_set_src(ui_ImageSettingStageLanguage, &ui_img_images_stage_green_png);
    lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);

#if defined(AQI_FACTORY_ONLY_CONFIG_WIFI)
    lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_green_png);
    lv_obj_add_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(ui_ButtonSettingNext, LV_ALIGN_CENTER);

    _aqi_factory.machine.State = &_aqi_factory_states[aqi_factory_state_wifi];
    __aqi_factory_state_wifi_entry_handler((state_machine_t *)&_aqi_factory);
#else
    _aqi_factory.machine.State = &_aqi_factory_states[aqi_factory_state_language];
    __aqi_factory_state_language_entry_handler((state_machine_t *)&_aqi_factory);
#endif
}

/********************************** Language State **********************************/
static state_machine_result_t __aqi_factory_state_language_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to language state");

    lv_obj_clear_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(ui_ButtonSettingNext, LV_ALIGN_CENTER);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_language_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_FACTORY_EVENT_NEXT:
        {
            return switch_state(pState, &_aqi_factory_states[aqi_factory_state_country]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_language_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from language state");

    lv_obj_add_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(ui_ButtonSettingNext, LV_ALIGN_RIGHT_MID);

    return EVENT_HANDLED;
}

/********************************** Country State **********************************/
static state_machine_result_t __aqi_factory_state_country_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to country state");

    lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_green_png);
    lv_obj_clear_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_country_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_FACTORY_EVENT_BACK:
        {
            lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_grey_png);
            return switch_state(pState, &_aqi_factory_states[aqi_factory_state_language]);
        }

        case AQI_FACTORY_EVENT_NEXT:
        {
            return switch_state(pState, &_aqi_factory_states[aqi_factory_state_wifi]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_country_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from country state");

    lv_obj_add_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Wifi State **********************************/
static state_machine_result_t __aqi_factory_state_wifi_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to wifi state");

    lv_img_set_src(ui_ImageSettingStageWifi, &ui_img_images_stage_green_png);

    lv_obj_clear_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    aqi_wifi_event(aqi_wifi_event_config);

    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_wifi_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case AQI_FACTORY_EVENT_BACK:
        {
            lv_img_set_src(ui_ImageSettingStageWifi, &ui_img_images_stage_grey_png);
            return switch_state(pState, &_aqi_factory_states[aqi_factory_state_country]);
        }

        case AQI_FACTORY_EVENT_NEXT:
        {
            if (aqi_wifi_exit_condition())
            {
                switch_state(pState, &_aqi_factory_states[aqi_factory_state_done]);
                return TRIGGERED_TO_SELF;
            }
            break;
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_wifi_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from wifi state");

    lv_obj_add_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    aqi_wifi_event(aqi_wifi_event_setting_done);

    return EVENT_HANDLED;
}

/********************************** Done State **********************************/
static state_machine_result_t __aqi_factory_state_done_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to done state");
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_done_handler(state_machine_t* const pState)
{
    aqi_factory_t* const aqi_factory = (aqi_factory_t*)pState;

    switch (pState->Event)
    {
        case AQI_FACTORY_EVENT_NEXT:
        {
            lv_obj_add_flag(ui_ContainerSettingButton, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);

            lv_scr_load_anim(ui_ScreenIndicatorAQI, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

            aqi_factory->setting_done = 1;
            // TODO: save to not setting factory for next time
            
            break;
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __aqi_factory_state_done_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from done state");
    return EVENT_HANDLED;
}
