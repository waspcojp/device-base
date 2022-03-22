#ifndef	__INC_SENSORS_H__
#define	__INC_SENSORS_H__

#include	"SensorInfo.h"

class	Sensors
{
  public:
	static	void	init(void);
	static	void	start(void);
	static	void	stop(int sec);
	static	void	collect(time_t n, SenseBuffer *buff);
	static	void	collect(time_t n, SenseBuffer *buff, SensorInfo *info);
	static	int		count(void)	{
		return	(nSensors);
	};
	static	SensorInfo	*find(const char *name)	{
		for	( int i = 0; i < nSensors; i ++ )	{
			if	( !strcmp(_Sensors[i]->name(), name) ) {
				return	(_Sensors[i]);
			}
		}
		return	(NULL);
	};
	static	SensorInfo	*item(uint8_t i)	{
		return	(_Sensors[i]);
	};
	static	SensorInfo	*add(SensorInfo *sensor)	{
		sensor->id = nSensors;
		_Sensors[nSensors ++] = sensor;
		ESP_LOGI("","%s sensor id = %d", sensor->name(), (int)sensor->id);
		return	(sensor);
	};
	static	void	iter(void(*func)(int index, SensorInfo *info))	{
		for	( int i = 0; i < nSensors; i ++)	{
			func(i, _Sensors[i]);
		}
	}

  protected:
	static	uint8_t		nSensors;
	static	SensorInfo	*_Sensors[NR_SENSORS];
};

#endif
