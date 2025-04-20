#include "./lv_i18n.h"


////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

#define UNUSED(x) (void)(x)

static inline uint32_t op_n(int32_t val) { return (uint32_t)(val < 0 ? -val : val); }
static inline uint32_t op_i(uint32_t val) { return val; }
// always zero, when decimal part not exists.
static inline uint32_t op_v(uint32_t val) { UNUSED(val); return 0;}
static inline uint32_t op_w(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_f(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_t(uint32_t val) { UNUSED(val); return 0; }

static lv_i18n_phrase_t en_singulars[] = {
    {"setting", "Settings"},
    {"select_language", "Select Language"},
    {"select_country", "Select Country"},
    {"wifi_setting", "Wi-Fi Settings"},
    {"enter_password", "Enter Password"},
    {"language", "language"},
    {"country", "country"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Sound"},
    {"history", "History"},
    {"email_alert", "Email Alerts"},
    {"button_back", "Back"},
    {"button_next", "Next"},
    {"button_connect", "Connect"},
    {"button_select", "Select"},
    {"country_en", "United States"},
    {"language_en", "English"},
    {"country_de", "Germany"},
    {"language_de", "German"},
    {"country_es", "Spain"},
    {"language_es", "Spanish"},
    {"country_pt", "Portugal"},
    {"language_pt", "Portuguese"},
    {"country_pl", "Poland"},
    {"language_pl", "Polish"},
    {"country_be", "Belgium"},
    {"language_be", "Belgian"},
    {"country_tr", "Turkey"},
    {"language_tr", "Turkish"},
    {"country_sv", "Sweden"},
    {"language_sv", "Swedish"},
    {"country_ro", "Romania"},
    {"language_ro", "Romanian"},
    {NULL, NULL} // End mark
};



static uint8_t en_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_lang = {
    .locale_name = "en",
    .singulars = en_singulars,

    .locale_plural_fn = en_plural_fn
};

static lv_i18n_phrase_t be_singulars[] = {
    {"setting", "Paramètres"},
    {"select_language", "Sélectionner la langue"},
    {"select_country", "Sélectionner le pays"},
    {"wifi_setting", "Paramètres Wi-Fi"},
    {"enter_password", "Entrer le mot de passe"},
    {"language", "langue"},
    {"country", "pays"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Son"},
    {"history", "Historique"},
    {"email_alert", "Alertes par e-mail"},
    {"button_back", "Retour"},
    {"button_next", "Suivant"},
    {"button_connect", "Connecter"},
    {"button_select", "Sélectionner"},
    {"country_en", "États-Unis"},
    {"language_en", "Anglais"},
    {"country_de", "Allemagne"},
    {"language_de", "Allemand"},
    {"country_es", "Espagne"},
    {"language_es", "Espagnol"},
    {"country_pt", "Portugal"},
    {"language_pt", "Portugais"},
    {"country_pl", "Pologne"},
    {"language_pl", "Polonais"},
    {"country_be", "Belgique"},
    {"language_be", "Belge"},
    {"country_tr", "Turquie"},
    {"language_tr", "Turc"},
    {"country_sv", "Suède"},
    {"language_sv", "Suédois"},
    {"country_ro", "Roumanie"},
    {"language_ro", "Roumain"},
    {NULL, NULL} // End mark
};



static uint8_t be_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);

    uint32_t n10 = n % 10;
    uint32_t n100 = n % 100;
    if ((n10 == 1 && n100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if (((2 <= n10 && n10 <= 4) && (!(12 <= n100 && n100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((n10 == 0) || ((5 <= n10 && n10 <= 9)) || ((11 <= n100 && n100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t be_lang = {
    .locale_name = "be",
    .singulars = be_singulars,

    .locale_plural_fn = be_plural_fn
};

static lv_i18n_phrase_t de_singulars[] = {
    {"setting", "Einstellungen"},
    {"select_language", "Sprache auswählen"},
    {"select_country", "Land auswählen"},
    {"wifi_setting", "WLAN-Einstellungen"},
    {"enter_password", "Passwort eingeben"},
    {"language", "Sprache"},
    {"country", "Land"},
    {"wifi", "WLAN"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Ton"},
    {"history", "Verlauf"},
    {"email_alert", "E-Mail-Benachrichtigungen"},
    {"button_back", "Zurück"},
    {"button_next", "Weiter"},
    {"button_connect", "Verbinden"},
    {"button_select", "Auswählen"},
    {"country_en", "Vereinigte Staaten"},
    {"language_en", "Englisch"},
    {"country_de", "Deutschland"},
    {"language_de", "Deutsch"},
    {"country_es", "Spanien"},
    {"language_es", "Spanisch"},
    {"country_pt", "Portugal"},
    {"language_pt", "Portugiesisch"},
    {"country_pl", "Polen"},
    {"language_pl", "Polnisch"},
    {"country_be", "Belgien"},
    {"language_be", "Belgisch"},
    {"country_tr", "Türkei"},
    {"language_tr", "Türkisch"},
    {"country_sv", "Schweden"},
    {"language_sv", "Schwedisch"},
    {"country_ro", "Rumänien"},
    {"language_ro", "Rumänisch"},
    {NULL, NULL} // End mark
};



static uint8_t de_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t de_lang = {
    .locale_name = "de",
    .singulars = de_singulars,

    .locale_plural_fn = de_plural_fn
};

static lv_i18n_phrase_t es_singulars[] = {
    {"setting", "Configuración"},
    {"select_language", "Seleccionar Idioma"},
    {"select_country", "Seleccionar País"},
    {"wifi_setting", "Configuración de Wi-Fi"},
    {"enter_password", "Introducir Contraseña"},
    {"language", "idioma"},
    {"country", "país"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Sonido"},
    {"history", "Historial"},
    {"email_alert", "Alertas de Correo"},
    {"button_back", "Atrás"},
    {"button_next", "Siguiente"},
    {"button_connect", "Conectar"},
    {"button_select", "Seleccionar"},
    {"country_en", "Estados Unidos"},
    {"language_en", "Inglés"},
    {"country_de", "Alemania"},
    {"language_de", "Alemán"},
    {"country_es", "España"},
    {"language_es", "Español"},
    {"country_pt", "Portugal"},
    {"language_pt", "Portugués"},
    {"country_pl", "Polonia"},
    {"language_pl", "Polaco"},
    {"country_be", "Bélgica"},
    {"language_be", "Belga"},
    {"country_tr", "Turquía"},
    {"language_tr", "Turco"},
    {"country_sv", "Suecia"},
    {"language_sv", "Sueco"},
    {"country_ro", "Rumanía"},
    {"language_ro", "Rumano"},
    {NULL, NULL} // End mark
};



static uint8_t es_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);


    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t es_lang = {
    .locale_name = "es",
    .singulars = es_singulars,

    .locale_plural_fn = es_plural_fn
};

static lv_i18n_phrase_t pl_singulars[] = {
    {"setting", "Ustawienia"},
    {"select_language", "Wybierz język"},
    {"select_country", "Wybierz kraj"},
    {"wifi_setting", "Ustawienia Wi-Fi"},
    {"language", "język"},
    {"country", "kraj"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Dźwięk"},
    {"history", "Historia"},
    {"email_alert", "Powiadomienia e-mail"},
    {"enter_password", "Wprowadź hasło"},
    {"button_back", "Wstecz"},
    {"button_next", "Dalej"},
    {"button_connect", "Połącz"},
    {"button_select", "Wybierz"},
    {"country_en", "Stany Zjednoczone"},
    {"language_en", "Angielski"},
    {"country_de", "Niemcy"},
    {"language_de", "Niemiecki"},
    {"country_es", "Hiszpania"},
    {"language_es", "Hiszpański"},
    {"country_pt", "Portugalia"},
    {"language_pt", "Portugalski"},
    {"country_pl", "Polska"},
    {"language_pl", "Polski"},
    {"country_be", "Belgia"},
    {"language_be", "Belgijski"},
    {"country_tr", "Turcja"},
    {"language_tr", "Turecki"},
    {"country_sv", "Szwecja"},
    {"language_sv", "Szwedzki"},
    {"country_ro", "Rumunia"},
    {"language_ro", "Rumuński"},
    {NULL, NULL} // End mark
};



static uint8_t pl_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i != 1 && (0 <= i10 && i10 <= 1)) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (12 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pl_lang = {
    .locale_name = "pl",
    .singulars = pl_singulars,

    .locale_plural_fn = pl_plural_fn
};

static lv_i18n_phrase_t pt_singulars[] = {
    {"setting", "Configurações"},
    {"select_language", "Selecionar Idioma"},
    {"select_country", "Selecionar País"},
    {"wifi_setting", "Configurações de Wi-Fi"},
    {"language", "idioma"},
    {"country", "país"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Som"},
    {"history", "Histórico"},
    {"email_alert", "Alertas de Email"},
    {"enter_password", "Inserir Senha"},
    {"button_back", "Voltar"},
    {"button_next", "Próximo"},
    {"button_connect", "Conectar"},
    {"button_select", "Selecionar"},
    {"country_en", "Estados Unidos"},
    {"language_en", "Inglês"},
    {"country_de", "Alemanha"},
    {"language_de", "Alemão"},
    {"country_es", "Espanha"},
    {"language_es", "Espanhol"},
    {"country_pt", "Portugal"},
    {"language_pt", "Português"},
    {"country_pl", "Polônia"},
    {"language_pl", "Polonês"},
    {"country_be", "Bélgica"},
    {"language_be", "Belga"},
    {"country_tr", "Turquia"},
    {"language_tr", "Turco"},
    {"country_sv", "Suécia"},
    {"language_sv", "Sueco"},
    {"country_ro", "Romênia"},
    {"language_ro", "Romeno"},
    {NULL, NULL} // End mark
};



static uint8_t pt_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);

    if (((0 <= i && i <= 1))) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t pt_lang = {
    .locale_name = "pt",
    .singulars = pt_singulars,

    .locale_plural_fn = pt_plural_fn
};

static lv_i18n_phrase_t ro_singulars[] = {
    {"setting", "Setări"},
    {"select_language", "Selectează Limba"},
    {"select_country", "Selectează Țara"},
    {"wifi_setting", "Setări Wi-Fi"},
    {"language", "limbă"},
    {"country", "țară"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Sunet"},
    {"history", "Istoric"},
    {"email_alert", "Alerte Email"},
    {"button_back", "Înapoi"},
    {"button_next", "Înainte"},
    {"button_connect", "Conectare"},
    {"button_select", "Selectează"},
    {"country_en", "Statele Unite"},
    {"language_en", "Engleză"},
    {"country_de", "Germania"},
    {"language_de", "Germană"},
    {"country_es", "Spania"},
    {"language_es", "Spaniolă"},
    {"country_pt", "Portugalia"},
    {"language_pt", "Portugheză"},
    {"country_pl", "Polonia"},
    {"language_pl", "Poloneză"},
    {"country_be", "Belgia"},
    {"language_be", "Belgiană"},
    {"country_tr", "Turcia"},
    {"language_tr", "Turcă"},
    {"country_sv", "Suedia"},
    {"language_sv", "Suedeză"},
    {"country_ro", "România"},
    {"language_ro", "Română"},
    {"enter_password", "Introduceți parola"},
    {NULL, NULL} // End mark
};



static uint8_t ro_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t n100 = n % 100;
    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v != 0) || (n == 0) || (n != 1 && (1 <= n100 && n100 <= 19))) return LV_I18N_PLURAL_TYPE_FEW;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ro_lang = {
    .locale_name = "ro",
    .singulars = ro_singulars,

    .locale_plural_fn = ro_plural_fn
};

static lv_i18n_phrase_t sv_singulars[] = {
    {"setting", "Inställningar"},
    {"select_language", "Välj språk"},
    {"select_country", "Välj land"},
    {"wifi_setting", "Wi-Fi-inställningar"},
    {"language", "språk"},
    {"country", "land"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Ljud"},
    {"history", "Historik"},
    {"email_alert", "E-postaviseringar"},
    {"enter_password", "Ange lösenord"},
    {"button_back", "Tillbaka"},
    {"button_next", "Nästa"},
    {"button_connect", "Anslut"},
    {"button_select", "Välj"},
    {"country_en", "USA"},
    {"language_en", "Engelska"},
    {"country_de", "Tyskland"},
    {"language_de", "Tyska"},
    {"country_es", "Spanien"},
    {"language_es", "Spanska"},
    {"country_pt", "Portugal"},
    {"language_pt", "Portugisiska"},
    {"country_pl", "Polen"},
    {"language_pl", "Polska"},
    {"country_be", "Belgien"},
    {"language_be", "Belgiska"},
    {"country_tr", "Turkiet"},
    {"language_tr", "Turkiska"},
    {"country_sv", "Sverige"},
    {"language_sv", "Svenska"},
    {"country_ro", "Rumänien"},
    {"language_ro", "Rumänska"},
    {NULL, NULL} // End mark
};



static uint8_t sv_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t sv_lang = {
    .locale_name = "sv",
    .singulars = sv_singulars,

    .locale_plural_fn = sv_plural_fn
};

static lv_i18n_phrase_t tr_singulars[] = {
    {"setting", "Ayarlar"},
    {"select_language", "Dil Seç"},
    {"select_country", "Ülke Seç"},
    {"wifi_setting", "Wi-Fi Ayarları"},
    {"enter_password", "Şifre Gir"},
    {"language", "dil"},
    {"country", "ülke"},
    {"wifi", "Wi-Fi"},
    {"bluetooth", "Bluetooth"},
    {"sound", "Ses"},
    {"history", "Geçmiş"},
    {"email_alert", "E-posta Uyarıları"},
    {"button_back", "Geri"},
    {"button_next", "İleri"},
    {"button_connect", "Bağlan"},
    {"button_select", "Seç"},
    {"country_en", "Amerika Birleşik Devletleri"},
    {"language_en", "İngilizce"},
    {"country_de", "Almanya"},
    {"language_de", "Almanca"},
    {"country_es", "İspanya"},
    {"language_es", "İspanyolca"},
    {"country_pt", "Portekiz"},
    {"language_pt", "Portekizce"},
    {"country_pl", "Polonya"},
    {"language_pl", "Lehçe"},
    {"country_be", "Belçika"},
    {"language_be", "Belçika dili"},
    {"country_tr", "Türkiye"},
    {"language_tr", "Türkçe"},
    {"country_sv", "İsveç"},
    {"language_sv", "İsveççe"},
    {"country_ro", "Romanya"},
    {"language_ro", "Romence"},
    {NULL, NULL} // End mark
};



static uint8_t tr_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);


    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t tr_lang = {
    .locale_name = "tr",
    .singulars = tr_singulars,

    .locale_plural_fn = tr_plural_fn
};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_lang,
    &be_lang,
    &de_lang,
    &es_lang,
    &pl_lang,
    &pt_lang,
    &ro_lang,
    &sv_lang,
    &tr_lang,
    NULL // End mark
};

////////////////////////////////////////////////////////////////////////////////


// Internal state
static const lv_i18n_language_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if(strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


static const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;

    // Search in current locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;
    lv_i18n_plural_type_t ptype;

    // Search in current locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}
