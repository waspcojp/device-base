idf_component_register(SRCS
        device.cpp 
    INCLUDE_DIRS "." "../includes" "../core"
                "../sensors" "../drivers" "../actuators"
    REQUIRES esp_http_client esp_http_server nvs_flash json esp-tls app_update esp_https_ota)