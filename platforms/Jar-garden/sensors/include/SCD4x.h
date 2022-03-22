/**
 * @file SCD4x.h
 * @brief SCD4x CO2 センサドライバ
 */
#ifndef _SCD4X_H_
#define _SCD4X_H_

#include "I2C.h"

#include	"SensorInfo.h"
#include	"SenseBuffer.h"

#include "BME280_Barometric.h"
#include <stdint.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class SCD4x {
private:
        /** SCD4x コマンド一覧 */
        enum {
                CMD_START_PERIODIC_MEASUREMENT                  = 0x21b1,
                CMD_READ_MEASUREMENT                            = 0xec05,
                CMD_STOP_PERIODIC_MEASUREMENT                   = 0x3f86,

                CMD_SET_TEMPERATURE_OFFSET                      = 0x241d,
                CMD_GET_TEMPERATURE_OFFSET                      = 0x2318,
                CMD_SET_SENSOR_ALTITUDE                         = 0x2427,
                CMD_GET_SENSOR_ALTITUDE                         = 0x2322,
                CMD_SET_AMBIENT_PRESSURE                        = 0x0e00,

                CMD_PERFORM_FORCED_RECALIBRATION                = 0x362f,
                CMD_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED      = 0x2416,
                CMD_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED      = 0x2313,

                CMD_START_LOW_POWER_PERIODIC_MEASUREMENT        = 0x21ac,
                CMD_GET_DATA_READY_STATUS                       = 0xe4b8,

                CMD_PERSIST_SETTINGS                            = 0x3615,
                CMD_GET_SERIAL_NUMBER                           = 0x3682,
                CMD_PERFORM_SELF_TEST                           = 0x3639,
                CMD_PERFORM_FACTORY_RESET                       = 0x3632,
                CMD_REINIT                                      = 0x3646,

                CMD_MEASURE_SINGLE_SHOT                         = 0x219d,
                CMD_MEASURE_SINGLE_SHOT_RHT_ONLY                = 0x2196,
        };
        I2C                     *i2c;
        BME280_Barometric       *bme280;
        float   sensor_temp, sensor_hum, sensor_co2;
        uint8_t crc ( const uint8_t *data, uint16_t count );

        esp_err_t comm_i2c ( const uint16_t cmd,
                const uint16_t *xbuf, const size_t xlen,
                const int wait_ms,
                uint16_t *rbuf, const size_t rlen );
public:
        enum {
                I2C_ADDRESS = 0x62,
        };
        SCD4x ( I2C *i2c_param, BME280_Barometric *bme280_param );
        ~SCD4x ( void );
        esp_err_t       measure ( void );
        esp_err_t       set_pressure ( const float hpa );
        esp_err_t       calibrate ( const float ppm_co2 );
        esp_err_t       factory_reset ( void );
        float get_temp ( void ) { return sensor_temp; }
        float get_hum ( void ) { return sensor_hum; }
        float get_co2 ( void ) { return sensor_co2; }

};

#endif /* _SCD4X_H_ */