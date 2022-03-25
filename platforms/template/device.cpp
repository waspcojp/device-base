#define	TRACE

extern "C" {
#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include 	"config.h"
#include	<nvs.h>
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include	<time.h>
#include	<sys/time.h>
#include	<driver/ledc.h>
#include	<cJSON.h>
#include	"neopixel.h"
#include	"nvs.h"
#include	"wifi.h"
#include	"types.h"
#include	"misc.h"
#include	"api_client.h"
#include	"httpc.h"
#include	"schedule_funcs.h"
#include	"ota.h"
#include	"globals.h"
#include 	"debug.h"
}
#include	"device.h"
#include	"utility.h"

#include	"SensorInfo.h"
#include	"Sensors.h"
#include	"sensor.h"

extern	void
show_status(
	int		status)
{
}
#ifdef	HAVE_SENSORS
static	Bool
check_sensor(
	tTime	*now)
{
	int		at;
	static	int		last;
	
	if		( now == NULL )	{
		last = 0;
	} else {
		at = now->tm_hour * 3600 + now->tm_min * 60 + now->tm_sec;
		if		( last > ( at % sensor_interval ) )	{
			show_time(now, "sensor data collection");
			sensor_collect();
		}
		last = at % sensor_interval;
	}
	return	(FALSE);
}

#ifdef	USE_WIFI
static	Bool
check_send(
	tTime	*now)
{
	static	int		send_min;
	static	Bool	sent;
	
	if		( now == NULL )	{
		send_min = random() % 59 + 1;
		dbgprintf("sensor data send every %d min", send_min);
		sent = FALSE;
	} else {
		if		( now->tm_min == send_min )	{
			if		( !sent )	{
				show_time(now, "send device data");
				sent = sensor_send_server();
			}
		} else {
			sent = FALSE;
		}
	}
	return	(FALSE);
}
#endif
#endif

extern	void
get_settings(void)
{
ENTER_FUNC;
LEAVE_FUNC;
}

#ifdef	HAVE_SENSORS
static	void
initialize_sensors(void)
{
	initialize_sensors_common();
	Sensors::init();
}
#endif
extern	void
initialize_device(void)
{
#ifdef	HAVA_SENSORS
	initialize_sensors();
	start_sensors();
#endif
}

extern	void
initialize_schedules(void)
{
#ifdef	USE_WIFI
	if	( wifi_is_valid() )	{
#ifdef	USE_OTA
		register_schedule_func(check_ota);
#endif
	}
#endif
#ifdef	HAVE_SENSORS
#ifdef	USE_WIFI
	register_schedule_func(check_send);
#endif
	register_schedule_func(check_sensor);
#endif
}

extern	void
finalize_schedules(void)
{
#ifdef	HAVE_SENSORS
	sensor_send_server();
#endif
}
