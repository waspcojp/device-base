#define	TRACE

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
#include    "ota.h"
#ifdef	USE_WIFI
#include	"api_client.h"
#endif
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"

#include	"utility.h"

#ifdef  USE_OTA
extern	Bool
check_ota(
	tTime	*now)
{
	static	int		ota_min;
	int		at;
	static	Bool	done;
	Bool	ret;

	ret = FALSE;
	if		( now == NULL )	{
		ota_min = random() % 1438 + 1;
		dbgprintf("OTA check  at %02d:%02d", ota_min / 60, ota_min % 60);
		done = FALSE;
	} else {
		at = now->tm_hour * 60 + now->tm_min;
		ota_min = at - 1;
		if		(	( !done )
				&&	( at > ota_min ) )	{
#ifdef	USE_WIFI
			ret = api_exec_ota();
#else
			ret = FALSE;
#endif
			done = TRUE;
		} else {
			if	( at < ota_min )	{
				done = FALSE;
			}
		}
	}
	return	(ret);
}
#endif
