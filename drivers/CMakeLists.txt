idf_component_register(SRCS 
        wifi.c
        bt.c
        I2C.cpp
        UART2.cpp
        SPI.cpp
        neopixel.c
    INCLUDE_DIRS "." "../includes" "../core"
    REQUIRES bt driver)