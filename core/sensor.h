#ifndef	__INC_SENSOR_H__
#define	__INC_SENSOR_H__

#include	"types.h"
#include        "SensorInfo.h"

extern	void	initialize_sensors_common(void);
extern	void	start_sensors(void);
extern	Bool	sensor_send_server(void);
extern  Bool	sensor_collect(void);
extern  Bool	sensor_collect(SensorInfo *base);

extern  Bool    post_uart2 ( void );

#endif
