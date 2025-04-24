
#include "aqi_language.h"

#include "aqi_selector.h"
#include "aqi_country.h"

#include "ui.h"

#include "esp_err.h"
#include "esp_log.h"

#define AQI_LANGUAGE_NUMBER AQI_SELECTOR_MAX
#define AQI_LANGUAGE_TYPE "language"

#define AQI_LANGUAGE_CONTAINER_SIZE   736, 256
#define AQI_LANGUAGE_CONTAINER_OFFSET 0, 68

#define AQI_LANGUAGE_ATTR_DEFAULT(xx) {\
    .selector = NULL,\
    .flag = &ui_img_images_flag_##xx##_png,\
    .flag_tab = &ui_img_images_flag_##xx##_44_png,\
    .locale = #xx\
}

typedef struct {
    aqi_selector_t *selector;
    const lv_img_dsc_t *flag;
    const lv_img_dsc_t *flag_tab;
    char *locale;
} aqi_language_attr_t;

typedef struct {
    uint8_t selected;
    lv_obj_t* container;
    aqi_language_attr_t attr[AQI_LANGUAGE_NUMBER];
} aqi_language_t;

static const char *TAG = "AQI LANGUAGE";

static aqi_language_t _aqi_language = {
    .selected = 0,
    .container = NULL,
    .attr = {
        AQI_LANGUAGE_ATTR_DEFAULT(en),
        AQI_LANGUAGE_ATTR_DEFAULT(de),
        AQI_LANGUAGE_ATTR_DEFAULT(es),
        AQI_LANGUAGE_ATTR_DEFAULT(pt),
        AQI_LANGUAGE_ATTR_DEFAULT(pl),
        AQI_LANGUAGE_ATTR_DEFAULT(be),
        AQI_LANGUAGE_ATTR_DEFAULT(tr),
        AQI_LANGUAGE_ATTR_DEFAULT(sv),
        AQI_LANGUAGE_ATTR_DEFAULT(ro)
    }
};

static void __aqi_language_update_ui(aqi_language_attr_t aqi_language_attr)
{
    lv_img_set_src(ui_ImageSettingTabLanguageIcon, aqi_language_attr.flag_tab);

    for (uint8_t i = 0; i < AQI_LANGUAGE_NUMBER; i++)
    {
        aqi_selector_update_language(_aqi_language.attr[i].selector, AQI_LANGUAGE_TYPE, _aqi_language.attr[i].locale);
    }

    lv_label_set_text_fmt(ui_LabelLoading, "%s...", _("loading"));

    lv_label_set_text(ui_LabelSettingTabTitleName, _("setting"));
    lv_label_set_text(ui_LabelSettingLanguageTitleName, _("select_language"));
    lv_label_set_text(ui_LabelSettingCountryTitleName, _("select_country"));
    lv_label_set_text(ui_LabelSettingWifiTitleName, _("wifi_setting"));

    lv_label_set_text(ui_LabelSettingWifiEnable, _("wifi"));
    lv_textarea_set_placeholder_text(ui_TextWifiPasswordType, _("enter_password"));

    lv_label_set_text_fmt(ui_LabelSettingTabLanguageName, "%s (%s)",
                          _(aqi_selector_label_name_locale(AQI_LANGUAGE_TYPE, aqi_language_attr.locale)), _("language"));
    lv_label_set_text(ui_LabelSettingTabWifiName, _("wifi"));

    lv_label_set_text(ui_LabelSettingTabBluetoothName, _("bluetooth"));
    lv_label_set_text(ui_LabelSettingTabSoundName, _("sound"));
    lv_label_set_text(ui_LabelSettingTabHistoryName, _("history"));
    lv_label_set_text(ui_LabelSettingTabEmailAlertName, _("email_alert"));

    lv_label_set_text(ui_LabelSettingBackButton, _("button_back"));
    lv_label_set_text(ui_LabelSettingNextButton, _("button_next"));
    lv_label_set_text(ui_LabelWifiConnectButton, _("button_connect"));
    lv_label_set_text(ui_LabelSettingSelectButton, _("button_select"));
}

static void __aqi_language_update(aqi_language_attr_t aqi_language_attr)
{
    lv_i18n_set_locale(aqi_language_attr.locale);

    __aqi_language_update_ui(aqi_language_attr);

    aqi_country_language_update();
}

static void __aqi_language_select_event_handler(lv_event_t *event)
{
    static aqi_selector_t* last_selector = NULL;
    aqi_selector_t* selector = lv_event_get_user_data(event);

    if (selector != last_selector)
    {
        // Update the last selected language
        if (last_selector != NULL)
        {
            aqi_selector_update_item(last_selector, 0);
        }
        last_selector = selector;

        // Update the selected language
        aqi_selector_update_item(selector, 1);
        _aqi_language.selected = selector->index;
        __aqi_language_update(_aqi_language.attr[_aqi_language.selected]);
    }
}

int aqi_language_init()
{
    lv_i18n_init(lv_i18n_language_pack);

    _aqi_language.container = aqi_selector_create_container(ui_ContainerSettingLanguage,
                                                            AQI_LANGUAGE_CONTAINER_SIZE, AQI_LANGUAGE_CONTAINER_OFFSET);

    for (uint8_t i = 0; i < AQI_LANGUAGE_NUMBER; i++)
    {
        _aqi_language.attr[i].selector = aqi_selector_create(i, _(aqi_selector_label_name_locale(AQI_LANGUAGE_TYPE, _aqi_language.attr[i].locale)),
                                                             _aqi_language.container, _aqi_language.attr[i].flag,
                                                             __aqi_language_select_event_handler);
        if (_aqi_language.attr[i].selector == NULL)
        {
            ESP_LOGE(TAG, "Failed to create language selector");
            return ESP_FAIL;
        }
    }

    lv_event_send(_aqi_language.attr[_aqi_language.selected].selector->item, LV_EVENT_CLICKED, NULL);

    return ESP_OK;
}
