
lv_i18n extract -s 'main/AQI/*.c' -t 'main/translations/*.yml'
lv_i18n compile -l en -t 'main/translations/*.yml' -o 'main/translations/src'
