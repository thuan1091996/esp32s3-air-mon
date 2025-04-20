
#include "aqi_factory.h"

#include "aqi_language.h"
#include "aqi_country.h"
#include "aqi_wifi.h"

#include "ui.h"

#include "hsm.h"

#include "esp_err.h"
#include "esp_log.h"

typedef enum {
    COUNTRY_STATE,
    LANGUAGE_STATE,
    WIFI_STATE
} aqi_factory_state_t;

typedef struct {
    state_machine_t machine;  //!< Abstract state machine
    uint32_t Set_Time;        //! Set time of a process
    uint32_t Resume_Time;     //!< Remaining time when the process is paused
    uint32_t Timer;           //!< Process timer
} aqi_factory_t;

static const char *TAG = "AQI FACTORY";

static state_machine_result_t __language_handler(state_machine_t* const State);
static state_machine_result_t __language_entry_handler(state_machine_t* const State);
static state_machine_result_t __language_exit_handler(state_machine_t* const State);

static state_machine_result_t __country_handler(state_machine_t* const State);
static state_machine_result_t __country_entry_handler(state_machine_t* const State);
static state_machine_result_t __country_exit_handler(state_machine_t* const State);

static state_machine_result_t __wifi_handler(state_machine_t* const State);
static state_machine_result_t __wifi_entry_handler(state_machine_t* const State);
static state_machine_result_t __wifi_exit_handler(state_machine_t* const State);

static const state_t _aqi_factory_states[] = {
    [LANGUAGE_STATE] = {
        .Handler = __language_handler,
        .Entry   = __language_entry_handler,
        .Exit    = __language_exit_handler,
    },
    [COUNTRY_STATE] = {
        .Handler = __country_handler,
        .Entry   = __country_entry_handler,
        .Exit    = __country_exit_handler,
    },
    [WIFI_STATE] = {
        .Handler = __wifi_handler,
        .Entry   = __wifi_entry_handler,
        .Exit    = __wifi_exit_handler,
    }
};

static aqi_factory_t _aqi_factory = {0};

void __aqi_factory_event(aqi_factory_event_t event)
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
        __aqi_factory_event(BACK);
    }
    else if (target == ui_ButtonSettingNext)
    {
        ESP_LOGI(TAG, "Next button clicked");
        __aqi_factory_event(NEXT);
    }
}

void aqi_factory_init()
{
    _aqi_factory.machine.State = &_aqi_factory_states[LANGUAGE_STATE];

    // Show the main container, buttons and stage 
    lv_obj_clear_flag(ui_ContainerSettingMain, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingButton, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);
    lv_img_set_src(ui_ImageSettingStageLanguage, &ui_img_images_stage_green_png);

    lv_obj_add_event_cb(ui_ButtonSettingBack, &__aqi_factory_event_handler, LV_EVENT_CLICKED, &_aqi_factory);
    lv_obj_add_event_cb(ui_ButtonSettingNext, &__aqi_factory_event_handler, LV_EVENT_CLICKED, &_aqi_factory);

    __language_entry_handler((state_machine_t *)&_aqi_factory);
}

void aqi_factory_button_clickable(bool clickable)
{
    if (clickable)
    {
        lv_obj_add_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(ui_ButtonSettingNext, LV_OBJ_FLAG_CLICKABLE);
    }
    else
    {
        lv_obj_clear_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(ui_ButtonSettingNext, LV_OBJ_FLAG_CLICKABLE);
    }
}

/********************************** Language State **********************************/
static state_machine_result_t __language_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to language state");

    lv_obj_clear_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(ui_ButtonSettingNext, LV_ALIGN_CENTER);

    return EVENT_HANDLED;
}

static state_machine_result_t __language_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case NEXT:
        {
            return switch_state(pState, &_aqi_factory_states[COUNTRY_STATE]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __language_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from language state");

    lv_obj_add_flag(ui_ContainerSettingLanguage, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_ButtonSettingBack, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_align(ui_ButtonSettingNext, LV_ALIGN_RIGHT_MID);

    return EVENT_HANDLED;
}

/********************************** Country State **********************************/
static state_machine_result_t __country_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to country state");

    lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_green_png);
    lv_obj_clear_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

static state_machine_result_t __country_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case BACK:
        {
            lv_img_set_src(ui_ImageSettingStageCountry, &ui_img_images_stage_grey_png);
            return switch_state(pState, &_aqi_factory_states[LANGUAGE_STATE]);
        }

        case NEXT:
        {
            return switch_state(pState, &_aqi_factory_states[WIFI_STATE]);
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __country_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from country state");

    lv_obj_add_flag(ui_ContainerSettingCountry, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}

/********************************** Wifi State **********************************/
static state_machine_result_t __wifi_entry_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Entering to wifi state");

    lv_img_set_src(ui_ImageSettingStageWifi, &ui_img_images_stage_green_png);
    lv_obj_clear_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);
    aqi_factory_button_clickable(false);

    aqi_wifi_event(aqi_wifi_event_config);

    return EVENT_HANDLED;
}

static state_machine_result_t __wifi_handler(state_machine_t* const pState)
{
    switch (pState->Event)
    {
        case BACK:
        {
            lv_img_set_src(ui_ImageSettingStageWifi, &ui_img_images_stage_grey_png);
            return switch_state(pState, &_aqi_factory_states[COUNTRY_STATE]);
        }

        case NEXT:
        {
            lv_obj_add_flag(ui_ContainerSettingButton, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_ContainerSettingStage, LV_OBJ_FLAG_HIDDEN);

            aqi_wifi_event(aqi_wifi_event_setting_done);

            lv_scr_load_anim(ui_ScreenIndicatorAQI, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
            break;
        }

        default:
            return EVENT_UN_HANDLED;
    }
    return EVENT_HANDLED;
}

static state_machine_result_t __wifi_exit_handler(state_machine_t* const pState)
{
    ESP_LOGI(TAG, "Exiting from wifi state");

    lv_obj_add_flag(ui_ContainerSettingWifi, LV_OBJ_FLAG_HIDDEN);

    return EVENT_HANDLED;
}
