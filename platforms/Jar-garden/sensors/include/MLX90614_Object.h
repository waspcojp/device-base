#ifndef __MLX90614_OBJECT_H__
#define __MLX90614_OBJECT_H__

#include    "I2C.h"

#include	"SensorInfo.h"
#include	"SenseBuffer.h"
#include	"MLX90614.h"

class MLX90614_Object : public MLX90614
{
  public:
	virtual	void	get(SenseBuffer *buff);
	virtual	const	char	*name(void);
	virtual	const	char	*data_class_name(void);
	virtual	float	value(void);
};
#endif