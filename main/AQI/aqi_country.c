
#include "aqi_country.h"

#include "aqi_selector.h"

#include "ui.h"

#include <time.h>

#include "esp_err.h"
#include "esp_log.h"

#define AQI_COUNTRY_NUMBER AQI_SELECTOR_MAX
#define AQI_COUNTRY_TYPE "country"

#define AQI_COUNTRY_CONTAINER_SIZE   736, 256
#define AQI_COUNTRY_CONTAINER_OFFSET 0, 68

#define AQI_COUNTRY_EN_TIMEZONE "EST5EDT,M3.2.0,M11.1.0"     // United States: Covers Eastern Time (DST starts 2nd Sunday in March, ends 1st Sunday in November).
#define AQI_COUNTRY_DE_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Germany: DST starts last Sunday of March and ends last Sunday of October.
#define AQI_COUNTRY_ES_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Spain: Same as Germany (shared European rules).
#define AQI_COUNTRY_PT_TIMEZONE "WET0WEST,M3.5.0,M10.5.0/3"  // Portugal: Western European Time (UTC), with DST.
#define AQI_COUNTRY_PL_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Poland: Follows European DST rules.
#define AQI_COUNTRY_BE_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Belgium: Same DST rules as Germany and Poland.
#define AQI_COUNTRY_TR_TIMEZONE "+3"                         // Turkey: does not observe DST since 2016. Fixed UTC+3.
#define AQI_COUNTRY_SV_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Sweden: Same DST rules as other EU countries.
#define AQI_COUNTRY_RO_TIMEZONE "EET-2EEST,M3.5.0,M10.5.0/3" // Romania: Eastern European Time with DST (same EU rules).

#define AQI_COUNTRY_ATTR_DEFAULT(XX, xx) {\
    .selector = NULL,\
    .flag = &ui_img_images_flag_##xx##_png,\
    .flag_tab = &ui_img_images_flag_##xx##_44_png,\
    .locale = #xx,\
    .timezone = AQI_COUNTRY_##XX##_TIMEZONE\
}

typedef struct {
    aqi_selector_t *selector;
    const lv_img_dsc_t *flag;
    const lv_img_dsc_t *flag_tab;
    char *locale;
    char *timezone;
} aqi_country_attr_t;

typedef struct {
    uint8_t selected;
    lv_obj_t* container;
    aqi_country_attr_t attr[AQI_COUNTRY_NUMBER];
} aqi_country_t;

static const char *TAG = "AQI COUNTRY";

static aqi_country_t _aqi_country = {
    .selected = 0,
    .container = NULL,
    .attr = {
        AQI_COUNTRY_ATTR_DEFAULT(EN, en),
        AQI_COUNTRY_ATTR_DEFAULT(DE, de),
        AQI_COUNTRY_ATTR_DEFAULT(ES, es),
        AQI_COUNTRY_ATTR_DEFAULT(PT, pt),
        AQI_COUNTRY_ATTR_DEFAULT(PL, pl),
        AQI_COUNTRY_ATTR_DEFAULT(BE, be),
        AQI_COUNTRY_ATTR_DEFAULT(TR, tr),
        AQI_COUNTRY_ATTR_DEFAULT(SV, sv),
        AQI_COUNTRY_ATTR_DEFAULT(RO, ro)
    }
};

static void __aqi_country_update_timezone(aqi_country_attr_t aqi_country_attr)
{
    ESP_LOGI(TAG, "Setting timezone for %s: %s",aqi_country_attr.locale, aqi_country_attr.timezone);

    setenv("TZ", _aqi_country.attr[_aqi_country.selected].timezone, 1);
    tzset();
}

static void __aqi_country_update(aqi_country_attr_t aqi_country_attr)
{
    lv_img_set_src(ui_ImageSettingTabCountryIcon, aqi_country_attr.flag_tab);

    __aqi_country_update_timezone(aqi_country_attr);

    lv_label_set_text_fmt(ui_LabelSettingTabCountryName, "%s (%s)",
                          _(aqi_selector_label_name_locale(AQI_COUNTRY_TYPE, aqi_country_attr.locale)), _("country"));
}

static void __aqi_country_select_event_handler(lv_event_t *event)
{
    static aqi_selector_t* last_selector = NULL;
    aqi_selector_t* selector = lv_event_get_user_data(event);

    if (selector != last_selector)
    {
        // Update the last selected country
        if (last_selector != NULL)
        {
            aqi_selector_update_item(last_selector, 0);
        }
        last_selector = selector;

        // Update the selected country
        aqi_selector_update_item(selector, 1);
        _aqi_country.selected = selector->index;
        __aqi_country_update(_aqi_country.attr[_aqi_country.selected]);
    }
}

int aqi_country_init()
{
    _aqi_country.container = aqi_selector_create_container(ui_ContainerSettingCountry,
                                                           AQI_COUNTRY_CONTAINER_SIZE, AQI_COUNTRY_CONTAINER_OFFSET);

    for (uint8_t i = 0; i < AQI_COUNTRY_NUMBER; i++)
    {
        _aqi_country.attr[i].selector = aqi_selector_create(i, _(aqi_selector_label_name_locale(AQI_COUNTRY_TYPE, _aqi_country.attr[i].locale)),
                                                            _aqi_country.container, _aqi_country.attr[i].flag,
                                                            __aqi_country_select_event_handler);
        if (_aqi_country.attr[i].selector == NULL)
        {
            ESP_LOGE(TAG, "Failed to create country selector");
            return ESP_FAIL;
        }
    }

    lv_event_send(_aqi_country.attr[_aqi_country.selected].selector->item, LV_EVENT_CLICKED, NULL);

    return ESP_OK;
}

int aqi_country_language_update()
{
    lv_label_set_text_fmt(ui_LabelSettingTabCountryName, "%s (%s)",
                          _(aqi_selector_label_name_locale(AQI_COUNTRY_TYPE, _aqi_country.attr[_aqi_country.selected].locale)), _("country"));

    for (uint8_t i = 0; i < AQI_COUNTRY_NUMBER; i++)
    {
        aqi_selector_update_language(_aqi_country.attr[i].selector, AQI_COUNTRY_TYPE, _aqi_country.attr[i].locale);
    }

    return ESP_OK;
}
