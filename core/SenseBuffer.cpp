#define	TRACE

extern "C" {
#include 	"esp_log.h"
}

#include	"config.h"
#include	"debug.h"
#include	"SensorInfo.h"
#include	"SenseBuffer.h"
#include	"Sensors.h"

#define	TAG	"SenseBuffer"

/*
uint8_t	SenseBuffer::_buff[SIZE_OF_SENSOR_BUFF];
uint8_t	*SenseBuffer::_w_pointer;
uint8_t	*SenseBuffer::_r_pointer;
*/
void
SenseBuffer::check_space(void)
{
	SensorInfo	*info;
	size_t		size
		,	shift;
	int		i;

ENTER_FUNC;
	size = 0;
	for( i = 0; i< Sensors::count(); i++ ){
		info = Sensors::item(i);
		if	( info->fValid )	{
			size += info->size() + 1;
		}
	}
	size ++;
	xSemaphoreTake(mutex, portMAX_DELAY);
	dbgprintf("use %d bytes left %d bytes", size, SIZE_OF_SENSOR_BUFF - (int)(_w_pointer - _buff));
	while	( ( SIZE_OF_SENSOR_BUFF - (int)( _w_pointer - _buff ) )  < (int)size )	{
		_r_pointer = _buff + sizeof(time_t);
		while	(( ( _r_pointer - _buff ) < SIZE_OF_SENSOR_BUFF ) &&
				 ( *_r_pointer != 0xFF ))	{
			info = Sensors::item((int)*_r_pointer);
			dbgprintf("name = [%s]", info->name());
			_r_pointer += info->size() + 1;
		}
		_r_pointer ++;
		shift = _r_pointer - _buff;
		dbgprintf("shift %d bytes", shift);
		memmove(_buff, _r_pointer, (SIZE_OF_SENSOR_BUFF - shift));
		_w_pointer -= shift;
	}
	*_r_pointer = 0xFF;
	xSemaphoreGive(mutex);
LEAVE_FUNC;
}

