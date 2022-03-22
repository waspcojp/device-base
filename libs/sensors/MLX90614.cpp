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

#define	TAG	"MLX90614"

MLX90614::~MLX90614()
{
}

size_t
MLX90614::build(
	char	*p,
	SenseBuffer	*buff)
{
	float	data;

ENTER_FUNC;
	buff->get(&data, sizeof(float));
LEAVE_FUNC;
	return	sprintf(p, "\"value\":%.2f,\"unit\":\"%s\"", data, unit());
}

const	char	*
MLX90614::unit(void)
{
	return	"degree Celsius";
}
