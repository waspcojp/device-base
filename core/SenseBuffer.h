#ifndef	__INC_SENSE_BUFFER_H__
#define	__INC_SENSE_BUFFER_H__

extern "C" {
#include	<stdlib.h>
#include	<stdint.h>
#include	<string.h>
#include	"config.h"
#include	"debug.h"
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/semphr.h>
}

class	SenseBuffer	{
  public:
	void	put(void *data, size_t size)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		memcpy(_w_pointer, data, size);
		_w_pointer += size;
		xSemaphoreGive(mutex);
	};
	void	get(void *data, size_t size)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		memcpy(data, _r_pointer, size);
		_r_pointer += size;
		xSemaphoreGive(mutex);
	};
	void	set_value(uint8_t data)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		*_w_pointer = data;
		_w_pointer ++;
		xSemaphoreGive(mutex);
	};
	uint8_t	current_value(void)	{
		return	(*_r_pointer);
	};
	uint8_t	get_value(void)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		uint8_t	ret = *_r_pointer;
		_r_pointer ++;
		xSemaphoreGive(mutex);
		return	(ret);
	};
	int		used_size(void)	{
		int	size;
		xSemaphoreTake(mutex, portMAX_DELAY);
		size = (int)(_w_pointer - _buff);
		xSemaphoreGive(mutex);
		return(size);
	};
	void	rewind_read(void)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		_r_pointer = _buff;
		xSemaphoreGive(mutex);
	}
	void	rewind_write(void)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		_w_pointer = _buff;
		xSemaphoreGive(mutex);
	}
	void	rewind_read(uint8_t *p)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		_r_pointer = p;
		xSemaphoreGive(mutex);
	}
	uint8_t	*mark_read(void)	{
		return	(_r_pointer);
	}
	bool	is_end(void)	{
		bool	ret;
		xSemaphoreTake(mutex, portMAX_DELAY);
		ret = (_r_pointer < _w_pointer) ? false : true;
		xSemaphoreGive(mutex);
		return	(ret);
	}
	uint8_t	*buff(void)	{
		return	(_buff);
	}
	uint8_t	*pointer(void)	{
		return	(_w_pointer);
	}
	void	set_pointer(uint32_t pointer)	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		_w_pointer = (uint8_t *)pointer;
		xSemaphoreGive(mutex);
	}
	void	check_space(void);
	SenseBuffer(size_t size)	{
		//_buff = (uint8_t *)malloc(SIZE_OF_SENSOR_BUFF);
		_buff = (uint8_t *)malloc(size);
		memset(_buff, 0xFF, size);
		if ( _buff == NULL ) {
			ESP_LOGI ( "SenseBuffer", "NULL pointer detected.");
		}
		_w_pointer = _r_pointer = _buff;
		mutex = xSemaphoreCreateMutex();
	}
	~SenseBuffer(void)	{
		free(_buff);
	}

  protected:
	uint8_t	*_buff;
	uint8_t	*_w_pointer;
	uint8_t	*_r_pointer;
  private:
	xSemaphoreHandle	mutex;
};

#endif
