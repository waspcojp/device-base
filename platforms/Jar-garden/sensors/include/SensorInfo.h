#ifndef	_INC_SENSOR_INFO_H_
#define	_INC_SENSOR_INFO_H_

#include	<stdlib.h>
#include	<stdint.h>
#include	<string.h>

extern "C" {
#include 	"esp_err.h"
}

#include	"SenseBuffer.h"

typedef	uint8_t		byte;

class SensorInfo
{
  public:
	virtual void	initialize(void){};
	virtual	void	get(SenseBuffer *buff) {};
	virtual	size_t	build(char *p, SenseBuffer *buff) { return 0; };
	virtual	void	stop(int sec) {};
	virtual	void	start(void) {};
	virtual	const	char	*name(void) { return NULL; };
	virtual	const	char	*unit(void) { return NULL; };
	virtual	const	char	*data_class_name(void) { return NULL; };
	virtual	size_t	size(void) { return sizeof(float); };
	virtual	float	value(void) { return 0.0; };
	bool	fValid;

	static	const	uint8_t	precision;
	esp_err_t	err_code;
	uint8_t		id;

	SensorInfo();
	~SensorInfo();
};

#endif
