/**
 * @file SPI.h
 * @brief SPI バスドライバ
 * 
 */
#ifndef _SPI_H_
#define _SPI_H_
extern "C" {
#include <hal/spi_hal.h>
#include <hal/spi_types.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
}
#include "hardware.h"

class SPI {
protected:
public:
        SPI(void);
        ~SPI(void);
        spi_device_handle_t     device_add ( spi_device_interface_config_t *conf );
        esp_err_t               device_remove ( spi_device_handle_t dev );
        esp_err_t               transaction ( spi_device_handle_t dev,
                                        uint8_t *txbuff, size_t txlen,
                                        uint8_t *rxbuff, size_t rxlen );
};

#endif /* _SPI_H_ */