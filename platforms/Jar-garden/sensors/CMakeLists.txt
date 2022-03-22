idf_component_register(SRCS
        BME280.cpp BME280_Barometric.cpp BME280_Humidity.cpp BME280_Temp.cpp
        MLX90614.cpp MLX90614_LED.cpp MLX90614_Object.cpp
    INCLUDE_DIRS "." "../includes" "../drivers" "../core"
    REQUIRES esp_http_client)
