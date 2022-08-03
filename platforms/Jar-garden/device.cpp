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
#ifdef	HAVE_LIGHT
#include	"light.h"
#endif
#ifdef	HAVE_COOLING_FAN
#include	"fan.h"
#endif
#include	"utility.h"

#include	"SensorInfo.h"
#include	"Sensors.h"
#include	"sensor.h"

#include	"I2C.h"
#include 	"BME280_Barometric.h"
#include 	"BME280_Humidity.h"
#include 	"BME280_Temp.h"

#ifdef	HAVE_MLX90614
#include	"MLX90614_Object.h"
#include	"MLX90614_LED.h"
#elif	defined(HAVE_BME290)
#include	"BME280_Temp.h"
#endif
#ifdef	HAVE_COOLING_FAN
static	FAN		*fan;
#endif
#ifdef	HAVE_LIGHT
static	Light	*light;
#endif

extern	void
show_status(
	int		status)
{
#ifdef	HAVE_LIGHT
	switch	(status)	{
	  case	STATUS_RUN:
		light->color(0, 0, 0, 0, 0);
		break;
	  case	STATUS_SETUP:
		light->color(0, 64, 128, 64, 0);
		break;
	  case	STATUS_SETUP_CONNECT:
		light->color(64, 64, 64,  64, 0);
		break;
	  case	STATUS_ALL_RESET:
		light->color(128, 0, 64, 64, 0);
		break;
	  case	STATUS_IDLE:
		light->color(128, 128, 0, 0, 0);
		//light->color(128, 128, 128, 128, 128);
		break;
	  case	STATUS_FAULT:
		light->color(128,   0, 0, 0, 0);
		break;
	}
#endif
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

#ifdef	HAVE_COOLING_FAN
static	Bool
check_fan(
	tTime	*now)
{
	static	SensorInfo	*sensor;
	float	temp;
	int		speed;

	if		( now == NULL )	{
#ifdef	HAVE_MLX90614
		sensor = (SensorInfo *)new MLX90614_LED();
#elif	defined(HAVE_BME290)
		sensor = (SensorInfo *)new BME280_Temp();
#endif
		dbgprintf("fan on  = %s", ftos(upper_temperature_limit, 0, 1));
		dbgprintf("fan off = %s", ftos(lower_temperature_limit, 0, 1));
	} else {
		if	( now->tm_sec == 0 )	{
			if		( sensor->fValid )	{
				temp = sensor->value();
				show_time(now, "fan check");
				dbgprintf("Temp = %s", ftos(temp, 0, 1));
				speed = fan->speed();
				if		(  temp > upper_temperature_limit )	{
					fan->set_switch(TRUE);
				}
				if		(  temp < lower_temperature_limit )	{
					fan->set_switch(FALSE);
				}
				if		( speed != fan->speed() )	{
					sensor_collect(fan);
				}
			} else {
				fan->set_switch(TRUE);
			}
		}
	}
	return	(FALSE);
}
#endif

typedef	struct	{
	int8_t	no;
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
	uint8_t	white;
	uint8_t	dred;
}	LED;
typedef	struct	{
	int		at;
	LED		led;
}	tScheduleEvent;

typedef	struct	{
	uint8_t		wday;
	uint8_t		nev;
	tScheduleEvent	*ev;
}	tSchedule;

static	tSchedule	Schedules[MAX_TIMER_EVENTS];
static	int			nSchedules;

/*
title: "schedule"
type: "array"
minItems: 1
maxItems: 10
items:
  type: "object"
  properties:
    day_of_week:
      type: "object"
      properties:
        Sun:
          type: "boolean"
        Mon:
          type: "boolean"
        Tue:
          type: "boolean"
        Wed:
          type: "boolean"
        Thu:
          type: "boolean"
        Fri:
          type: "boolean"
        Sat:
          type: "boolean"
    LED:
      type: "array"
      minItems: 1
      maxItems: 10
      items:
        type: "object"
        properties:
          at:
            type: "object"
            properties:
              hour:
                type: "integer"
              min:
                type: "integer"
              sec:
                type: "integer"
          color:
            type: "object"
            properties:
              number:
                type: "integer"
              red:
                type: "integer"
                format: "int32"
              green:
                type: "integer"
                format: "int32"
              blue:
                type: "integer"
                format: "int32"
              white:
                type: "integer"
                format: "int32"
              dred:
                type: "integer"
                format: "int32"
*/
static	Bool
check_wday(
	cJSON	*root,
	const char	*wday)
{
	cJSON	*node;

	node = cJSON_GetObjectItemCaseSensitive(root, wday);
	return	(	( node != NULL )
			&&	( cJSON_IsBool(node) )
			&&	( cJSON_IsTrue(node) ) );
}

static	void
reset_led_schedule(void)
{
	while ( nSchedules > 0 )	{
		nSchedules --;
		free(Schedules[nSchedules].ev);
	}
}

static	void
push_led_schedule(
	uint8_t	wday,
	int		nev,
	tScheduleEvent	*ev)
{
	Schedules[nSchedules].wday = wday;
	Schedules[nSchedules].nev = (uint8_t)nev;
	Schedules[nSchedules].ev = ev;
	nSchedules ++;
}

static	void
get_led_schedule(void)
{
	esp_http_client_handle_t	client;
	int		size
		,	i;
	cJSON	*root
		,	*node
		,	*entry
		,	*wday
		,	*led
		,	*event;
	uint8_t	sc_wday;
	tScheduleEvent	*ev = NULL;
	int		nev
		,	nsc;
	int		status;
	

ENTER_FUNC;
	reset_led_schedule();

	if	( !wifi_is_valid() )	{
		wifi_connect(NULL, NULL);
	}
	if	( wifi_is_valid () )	{
		client = initialize_httpc();
		for	( int i = 0; i < MAX_RETRY; i ++ )	{
			size = api_get_data(client, ENDPOINT_SCHEDULE,
					(uint8_t *)ResponseMessage, SIZE_RESPONSE_BUFFER, &status);
			dbgprintf("status = %d", status);
			switch	(status)	{
			case	403:
				api_relogin_device(client);
				size = -1;
				break;
			case	500:
				esp_restart();
				break;
			default:
				break;
			}
			if		( size < 0 )	{
				msleep(RETRY_INTERVAL * 1000);
			} else
				break;
		}
		dbgprintf("size = %d", size);
		ResponseMessage[size] = 0;
		httpc_close(client);
		finish_httpc(client);
	} else {
		dbgmsg("standalone mode");
		strcpy(ResponseMessage, "["
			"{"
				"\"day_of_week\": {"
					"\"Sun\": true,"
					"\"Mon\": true,"
					"\"Tue\": true,"
					"\"Wed\": true,"
					"\"Thu\": true,"
					"\"Fri\": true,"
					"\"Sat\": true"
				"},"
				"\"LED\": [{"
					"\"at\": \"00:00:00\","
					"\"number\": 0,"
					"\"color\": \"#404040\","
					"\"white\": 32,"
					"\"dred\": 100"
				"}]"
			"},{"
				"\"day_of_week\": {"
					"\"Sun\": true,"
					"\"Mon\": true,"
					"\"Tue\": true,"
					"\"Wed\": true,"
					"\"Thu\": true,"
					"\"Fri\": true,"
					"\"Sat\": true"
				"},"
				"\"LED\": [{"
					"\"at\": \"12:00:00\","
					"\"number\": 0,"
					"\"color\": \"#000000\","
					"\"white\": 0,"
					"\"dred\": 0"
				"}]"
			"}"
		"]");
	}
	dbgprintf("JSON: %s", ResponseMessage);
	root = cJSON_Parse(ResponseMessage);
 	ev = NULL;
	if	( root )	{
		nsc = 0;
		dbgprintf("type = %d", root->type);
		cJSON_ArrayForEach(entry, root)	{
			nsc ++;
		}
		dbgprintf("nsc = %d", nsc);
		cJSON_ArrayForEach(entry, root)	{
			wday = cJSON_GetObjectItemCaseSensitive(entry, "day_of_week");
			sc_wday = 0;
			if		( check_wday(wday, "Sun") )	{
				sc_wday |= 0x80;
			}
			if		( check_wday(wday, "Mon") )	{
				sc_wday |= 0x40;
			}
			if		( check_wday(wday, "Tue") )	{
				sc_wday |= 0x20;
			}
			if		( check_wday(wday, "Wed") )	{
				sc_wday |= 0x10;
			}
			if		( check_wday(wday, "Thr") )	{
				sc_wday |= 0x08;
			}
			if		( check_wday(wday, "Fri") )	{
				sc_wday |= 0x04;
			}
			if		( check_wday(wday, "Sat") )	{
				sc_wday |= 0x02;
			}
			led = cJSON_GetObjectItemCaseSensitive(entry, "LED");
			nev = 0;
			cJSON_ArrayForEach(event, led)	{
				nev ++;
			}
			dbgprintf("nev = %d", nev);
			ev = (tScheduleEvent *)malloc(nev * sizeof(tScheduleEvent));
			i = 0;
			cJSON_ArrayForEach(event, led)	{
				node = cJSON_GetObjectItemCaseSensitive(event, "at");
				ev[i].at = 0;
				if		(	( node )
						&&	( cJSON_IsString(node) ) )	{
					ev[i].at =
						  str2int(&node->valuestring[0], 2) * 3600
						+ str2int(&node->valuestring[3], 2) * 60
						+ str2int(&node->valuestring[6], 2);
				}
#if	( NR_LED > 1 )
				node = cJSON_GetObjectItemCaseSensitive(event, "number");
				ev[i].led.no = 0;
				if		(	( node )
						&&	( cJSON_IsNumber(node) ) )	{
					ev[i].led.no = node->valueint;
				}
#else
				ev[i].led.no = 0;
#endif
				node = cJSON_GetObjectItemCaseSensitive(event, "color");
				ev[i].led.white = 0;
				ev[i].led.red = 0;
				ev[i].led.green = 0;
				ev[i].led.blue = 0;
				ev[i].led.dred = 0;
				if		(	( node )
						&&	( cJSON_IsString(node) ) )	{
					ev[i].led.red = hex2int(&node->valuestring[1], 2);
					ev[i].led.green = hex2int(&node->valuestring[3], 2);
					ev[i].led.blue = hex2int(&node->valuestring[5], 2);
#if	( NR_COLOR > 4 )
					int	dred = 0;
					node = cJSON_GetObjectItemCaseSensitive(event, "dred");
					if		( node )	{
						if	( cJSON_IsNumber(node) )	{
							dred = node->valueint;
						} else
						if	( cJSON_IsString(node) )	{
							dred = atoi(node->valuestring);
						}
					}
					ev[i].led.dred = ( dred * (int)ev[i].led.red ) / 100;
					ev[i].led.red = ev[i].led.red - ev[i].led.dred;
#endif
				}
				ev[i].led.white = 0;
				node = cJSON_GetObjectItemCaseSensitive(event, "white");
				if		( node )	{
					if	( cJSON_IsNumber(node) )	{
						ev[i].led.white = node->valueint;
					} else
					if	( cJSON_IsString(node) )	{
						ev[i].led.white = atoi(node->valuestring);
					}
				}
				i ++;
			}
			push_led_schedule(sc_wday, nev, ev);
			for	( i = 0; i < nev ; i ++ )	{
				dbgprintf("%d (%d) %d %d %d %d %d",
						  (int)ev[i].at,
						  (int)ev[i].led.no,
						  (int)ev[i].led.red,
						  (int)ev[i].led.green,
						  (int)ev[i].led.blue,
						  (int)ev[i].led.white,
						  (int)ev[i].led.dred);
			}
		}
		cJSON_Delete(root);
	} else {
		dbgprintf("error in %s", cJSON_GetErrorPtr());
	}
LEAVE_FUNC;
}

static	void
led_clear(
	LED		*led)
{
	int		i;
	for	( i = 0 ; i < NR_LED ; i ++ )	{
		led[i].no = i;
		led[i].red = 0;
		led[i].green = 0;
		led[i].blue = 0;
		led[i].white = 0;
		led[i].dred = 0;
	}
}

static	void
current_status(
	tTime	*now,
	LED		*led)
{
	int		i
		,	j;
	tScheduleEvent	*ev;
	int		at
		,	no;

	at = now->tm_hour * 3600 + now->tm_min * 60 + now->tm_sec;
	led_clear(led);
	for	( i = 0; i < nSchedules; i ++ )	{
		if		( ( Schedules[i].wday & ( 0x80 >> now->tm_wday ) ) != 0 )	{
			ev = Schedules[i].ev;
			for	( j = 0 ; j < Schedules[i].nev ; j ++ )	{
				if		( ev[j].at <= at )	{
					if		( ( no = ev[j].led.no ) >= 0 )	{
						led[no] = ev[j].led;
					} else {
						for	( no = 0; no < NR_LED; no ++ )	{
							led[no] = ev[j].led;
							led[no].no = no;
						}
					}
				} else
					break;
			}
		}
	}
}

extern	Bool
check_led(
	tTime	*now)
{
	int		i;
	LED		led[NR_LED];
	static	LED	last_led[NR_LED];

	if	( now == NULL )	{
		nSchedules = 0;
		led_clear(last_led);
		get_led_schedule();
	} else {
		current_status(now, led);
		if		( memcmp(last_led, led, sizeof(LED) * NR_LED)  != 0 )	{
			show_time(now, "LED control");
			for	( i = 0 ; i < NR_LED ; i ++ )	{
				light->color(i,
					led[i].red,
					led[i].green,
					led[i].blue,
					led[i].white,
					led[i].dred);
				dbgprintf("%d: %d %d %d %d %d",
						  (int)led[i].no,
						  (int)led[i].red,
						  (int)led[i].green,
						  (int)led[i].blue,
						  (int)led[i].white,
						  (int)led[i].dred);
			}
			light->update();
			sensor_collect(light);
			memcpy(last_led, led, sizeof(LED) * NR_LED);
		}
	}
	return	(FALSE);
}

extern	Bool
check_led_schedule(
	tTime	*now)
{
	int		at;
	static	int		last;

	if	( now == NULL )	{
		last = 0;
	} else {
		at = now->tm_hour * 3600 + now->tm_min * 60 + now->tm_sec;
		if		( at - last > led_schedule_interval )	{
			if	( last > 0 )	{
				get_led_schedule();
			}
			last = at;
			dbgprintf("last = %d", last);
		}
	}
	return	(FALSE);
}

extern	void
get_settings(void)
{
	esp_http_client_handle_t	client;
	int		size;
	cJSON	*root
		,	*node
		,	*sensor
		,	*control;
	int		status;

ENTER_FUNC;
	//	defaults
	sensor_interval = DEFAULT_SENSE_INTERVAL;
	setenv("TZ", "UTC", 1);
	sensor_url = NULL;
	upper_temperature_limit = 31.0;
	lower_temperature_limit = 30.0;
	led_schedule_interval = 3600;
	sensor_interval = 30;
#ifdef	USE_WIFI
	if	( wifi_is_valid() )	{
		client = initialize_httpc();
		for	( int i = 0; i < MAX_RETRY; i ++ )	{
			size = api_get_data(client, ENDPOINT_SETTINGS,
					(uint8_t *)ResponseMessage, SIZE_RESPONSE_BUFFER, &status);
			dbgprintf("status = %d", status);
			switch	(status)	{
			case	403:
				api_relogin_device(client);
				size = -1;
				break;
			case	500:
				esp_restart();
				break;
			default:
				break;
			}
			if		( size < 0 )	{
				msleep(RETRY_INTERVAL * 1000);
			} else
				break;
		}
		dbgprintf("size = %d", size);
		if	( size > 0 )	{
			ResponseMessage[size] = 0;
		} else {
			ResponseMessage[0] = 0;
		}

		root = cJSON_Parse(ResponseMessage);
		if	( root )	{
			node = cJSON_GetObjectItemCaseSensitive(root, "timezone");
			if		(	( node )
					&&	( cJSON_IsString(node) ) )	{
				char	buff[65];
				strcpy(buff, node->valuestring);
				if	( !strlcmp(buff, "UTC") )	{
					if	( buff[3] == '+' )	{
						buff[3] = '-';
					} else {
						buff[3] = '-';
					}
				}
				setenv("TZ", buff, 1);
				dbgprintf("TZ=%s", getenv("TZ"));
				tzset();
			}

			control = cJSON_GetObjectItemCaseSensitive(root, "control");
			if		(	( control )
					&&	( cJSON_IsObject(control) ) )	{
#ifdef	HAVE_FAN
				node = cJSON_GetObjectItemCaseSensitive(control, "upper_temperature_limit");
				if		(	( node )
						&&	( cJSON_IsNumber(node) ) )	{
					upper_temperature_limit = node->valuedouble;
				}
				node = cJSON_GetObjectItemCaseSensitive(control, "lower_temperature_limit");
				if		(	( node )
						&&	( cJSON_IsNumber(node) ) )	{
					lower_temperature_limit = node->valuedouble;
				}
#endif
#ifdef	HAVE_LIGHT
				node = cJSON_GetObjectItemCaseSensitive(control, "led_schedule_interval");
				if		(	( node )
						&&	( cJSON_IsNumber(node) ) )	{
					led_schedule_interval = node->valueint;
				}
#endif
			}
			sensor = cJSON_GetObjectItemCaseSensitive(root, "sensor");
			if		(	( sensor )
					&&	( cJSON_IsObject(sensor) ) )	{
				node = cJSON_GetObjectItemCaseSensitive(sensor, "interval");
				if		(	( node )
						&&	( cJSON_IsNumber(node) ) )	{
					sensor_interval = node->valueint;
				}
				node = cJSON_GetObjectItemCaseSensitive(sensor, "post_url");
				if		(	( node )
						&&	( cJSON_IsString(node) ) )	{
					if	( *node->valuestring != 0 )	{
						sensor_url = strdup(node->valuestring);
					}
				}
			}
			cJSON_Delete(root);
		}
		finish_httpc(client);
	}
#endif
LEAVE_FUNC;
}

static	void
initialize_sensors(void)
{
	initialize_sensors_common();

	Sensors::init();

	Sensors::add((SensorInfo*)new BME280_Barometric());
	Sensors::add((SensorInfo*)new BME280_Humidity());
	Sensors::add((SensorInfo*)new BME280_Temp());
	Sensors::add((SensorInfo *)new MLX90614_Object());
	Sensors::add((SensorInfo *)new MLX90614_LED());
}

extern	void
initialize_device(void)
{
	initialize_sensors();

#ifdef	HAVE_LIGHT
	light = new Light();
#ifdef	SEND_LIGHT_STATUS
	Sensors::add(light);
#endif
#endif

#ifdef	HAVE_COOLING_FAN
	fan = new FAN();
	Sensors::add(fan);
	dbgprintf("fan id = %d", (int)fan->id);
#endif
	start_sensors();
}

extern	void
initialize_schedules(void)
{
	if	( wifi_is_valid() )	{
#ifdef	USE_OTA
		register_schedule_func(check_ota);
#endif
	}
#ifdef	HAVE_LIGHT
	register_schedule_func(check_led);
	register_schedule_func(check_led_schedule);
#endif
#ifdef	HAVE_SENSORS
	register_schedule_func(check_send);
	register_schedule_func(check_sensor);
#endif
#ifdef	HAVE_COOLING_FAN
	register_schedule_func(check_fan);
#endif

}

extern	void
finalize_schedules(void)
{
	sensor_send_server();
}
