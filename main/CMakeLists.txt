#Add sources from ui directory
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c" "AQI/*.c" "translations/src/*.c")

idf_component_register(SRCS "main.c" ${SRC_UI}
                    INCLUDE_DIRS "." "ui" "AQI" "translations/src"
                    REQUIRES apc1_sensor scd41 esp_wifi nvs_flash spiffs json state_machine)