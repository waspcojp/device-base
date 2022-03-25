idf_component_register(SRCS
    misc.c
    ota.c
    schedule.cpp schedule_funcs.c
    nvs.c
    sntp.c utility.cpp
    api_client.c httpc.c
    setup_mode.cpp
    command.cpp command_wifi.cpp
    SensorInfo.cpp sensor.cpp SenseBuffer.cpp Sensors.cpp
testcode.cpp v_watch.cpp
INCLUDE_DIRS "." "../includes" "../main" "../drivers" "../sensors"
REQUIRES esp_http_client esp_http_server nvs_flash json esp-tls app_update esp_https_ota)