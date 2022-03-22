#ifndef __INC_JAR_GARDEN_H__
#define __INC_JAR_GARDEN_H__

#include    "types.h"

extern	Bool    check_led(tTime *now);
extern	Bool    check_led_schedule(tTime *now);
extern	void    get_settings(void);
extern	void    initialize_sensors(void);

#endif
