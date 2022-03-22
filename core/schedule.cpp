//#define	TRACE

extern	"C"	{
#include 	"config.h"
#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include 	<esp_log.h>
#include	<time.h>

#include	"types.h"
#include	"schedule_funcs.h"
#include	"schedule.h"
#ifdef	USE_WIFI
#include	"api_client.h"
#endif
#include	"schedule.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"
}
#include	"device.h"
#include	"utility.h"

extern	"C"	{

static	Bool	stop = FALSE;

static	void
schedule_loop(
	void	*args)
{
	time_t	timeNow;
	tTime	now;

	stop = FALSE;
	while	( !stop )	{
		time(&timeNow);
		localtime_r(&timeNow, &now);
#ifdef	TRACE
		show_time(&now, NULL);
#endif
		stop = check_schedule_funcs(&now);
		msleep(1000);
	}
	finalize_schedules();
}

extern	void
start_schedule(void)
{
	time_t	timeNow;
	tTime	now;

	if	( !stop )	{
		time(&timeNow);
		srandom(timeNow);
		check_schedule_funcs(NULL);
		(void)xTaskCreate(schedule_loop, "schedule_loop", 8192, NULL, 5, NULL);
	}
}

extern	void
stop_schedule(void)
{
	stop = TRUE;
}

}	//	extern C
