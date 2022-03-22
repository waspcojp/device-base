#define	TRACE
//#define DEBUG
#define	MAIN

#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
extern "C"{
#include 	"config.h"
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include	<time.h>
#include	<sys/time.h>
#include	"nvs.h"
#include	"misc.h"
#include	"wifi.h"
#include	"schedule_funcs.h"
#include	"api_client.h"
#include	"sntp.h"
#include	"schedule.h"
#include	"globals.h"
#include 	"debug.h"
}
#include	"setup_mode.h"
#ifdef	USE_COMMAND
#include	"command.h"
#endif
#include	"device.h"
#include <esp_heap_caps.h>

#ifdef USE_V_WATCH
#include	"v_watch.h"
#endif

#define	HEALTH_CHECK(check) if ( ( err = check ) != ESP_OK ) { goto err_stop; } else {  health_check &= ~err_mask ; err_mask >>= 1; }

static	void
start(void)
{
	msleep(1000);

	get_settings();
#ifdef	USE_WIFI
	if	( wifi_is_valid() )	{
		initialize_sntp();
		while	( !sntp_valid() )	{
			msleep(1000*10);
		}
	}
#endif
	init_schedule_func();
	initialize_schedules();
	dbgmsg( "all preparation is done");
	start_schedule();
#ifdef	USE_COMMAND
#ifndef USE_V_WATCH
	start_command();
#else
	start_v_watch();
#endif
#endif
}

extern "C" void
app_main (void)
{
	int		count_reset;
	uint8_t	health_check;

ENTER_FUNC;
	health_check = 0x0;

	initialize_nvs();

	initialize_device();

	load_device_info();
#ifdef	USE_WIFI
	initialize_wifi();
#endif

	initialize_api();

#ifdef DEBUG
	count_reset = 0;
#else
	open_reset();
	count_reset = get_reset();
	dbgprintf( "reset count %d", count_reset);
	count_reset ++;
	set_reset((int32_t)count_reset);
	commit_reset();
#endif

	switch(count_reset) {
	  case	3:	//	setup mode
		SetupMode();
		break;
	  case	5:	//	all reset
		destroy_device_info();
		show_status(STATUS_ALL_RESET);
		count_reset = 0;
		set_reset(count_reset);
		commit_reset();
		msleep(1000*10);
		esp_restart();
		break;
	  default:
		show_status(STATUS_IDLE);
		msleep(1000*10);
		count_reset = 0;
#ifndef	DEBUG
		set_reset(count_reset);
		commit_reset();
		close_reset();
#endif
		if	( health_check == 0 )	{
			/*	OK	*/
			show_status(STATUS_RUN);
		} else {
			show_status(STATUS_FAULT);
			goto	stop;
		}
		dbgprintf( "%u bytes free after system initialize", (uint32_t)heap_caps_get_free_size(MALLOC_CAP_8BIT));
#ifdef	USE_WIFI		
		wifi_connect(ap_ssid, ap_pass);
		if	( wifi_is_valid() )	{
			dbgmsg( "network is OK");
		} else {
			dbgmsg( "network is not work");
		}
#endif
		start();
		break;
	}
  stop:;
	//STOP;
}
