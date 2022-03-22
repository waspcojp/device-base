#define TRACE

extern "C" {
#include 	"esp_log.h"
}
#include	"config.h"
#include	"debug.h"
#include	"utility.h"

#include	"SensorInfo.h"
#include	"SenseBuffer.h"
#include	"MLX90614.h"
#include	"MLX90614_LED.h"

#define	TAG	"MLX90614"

float
MLX90614_LED::value(void)
{
    uint8_t tbuff[3];
    uint16_t    ttemp;
    float       temp;

ENTER_FUNC;
    i2c.read(MLX90614_TA, (byte *)tbuff, 2);
    err_code = i2c.err_code;
    if  ( err_code == ESP_OK ) {
        ttemp = uint16_t(tbuff[0]) | ( uint16_t(tbuff[1]) << 8 );
        temp = (float)ttemp * 0.02 - 273.15;
		dbgprintf("temp = %s", ftos(temp, 0, 1));
    } else {
        temp = FLT_MAX;
    }
LEAVE_FUNC;
    return  (temp);
}

void
MLX90614_LED::get(
    SenseBuffer *buff)
{
    float       temp;
ENTER_FUNC;
    temp = this->value();
    buff->put(&temp, sizeof(float));
LEAVE_FUNC;
}

const	char	*
MLX90614_LED::name(void)
{
	return	"MLX90614_LEDTemp";
}

const	char	*
MLX90614_LED::data_class_name(void)
{
	return	"ObjectTemperature";
}

