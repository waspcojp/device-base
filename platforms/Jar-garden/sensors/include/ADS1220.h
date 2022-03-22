/**
 * @file ADS1220.h
 * @brief ADS1220 ドライバ
 */
#ifndef _ADS1220_H_
#define _ADS1220_H_
#include "SPI.h"
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include        <driver/gpio.h>

/** ADS1220 コマンド */
enum {
        ADS1220_CMD_RESET       = 0b00000110,
        ADS1220_CMD_STARTSYNC   = 0b00001000,
        ADS1220_CMD_POWERDOWN   = 0b00000010,
        ADS1220_CMD_RDATA       = 0b00010000,
        ADS1220_CMD_RREG        = 0b00100000,
        ADS1220_CMD_WREG        = 0b01000000,
};


class ADS1220 {
protected:
        SPI                     *spi;
        spi_device_handle_t     dev;
        gpio_num_t              nDRDY;
        SemaphoreHandle_t       sem;

        esp_err_t       reg_read ( int regnum, uint8_t *val );
        esp_err_t       reg_write ( int regnum, uint8_t val );
        esp_err_t       regs_read ( uint8_t regs[4] );
        esp_err_t       regs_write ( const uint8_t regs[4] );
        esp_err_t       wait_drdy ( void );
        esp_err_t       lock ( void );
        void            unlock ( void ); 

public:
        ADS1220 ( SPI *spi_bus );
        ~ADS1220 ( void );

        esp_err_t       measure_raw ( int *val, const uint8_t regs[4] );
        esp_err_t       measure_temp ( float *val );
        esp_err_t       measure_volt ( float *val, const int ch );
};
#endif /* _ADS1220_H_ */