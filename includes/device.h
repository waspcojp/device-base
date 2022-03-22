#ifndef __INC_DEVICE_H__
#define __INC_DEVICE_H__

#include    "types.h"

extern	void    show_status(int status);
extern	void    get_settings(void);
extern	void    initialize_device(void);
extern	void	initialize_schedules(void);
extern	void	finalize_schedules(void);

#endif
