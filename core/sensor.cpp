#define	TRACE

#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
extern "C"{
#include 	"config.h"
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include 	<esp_log.h>
}
#include	"Sensors.h"
#include	"SensorInfo.h"
#include	"SenseBuffer.h"
#include	"sensor.h"
extern "C"{
#include	"types.h"
#include	"httpc.h"
#include	"wifi.h"
#include	"api_client.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"
}
#ifdef	USE_I2C
#include	"I2C.h"
#endif

#define	TAG	"sensor"

static	char	*message_pointer;
static	SenseBuffer	*sense_buff;

static	void
make_one_sensor_message(
	time_t	timeNow,
	SensorInfo	*info,
	SenseBuffer	*buff)
{
	char	*q = message_pointer;
	tTime	tm_now;

ENTER_FUNC;
	dbgprintf("name = %s", info->name());
	gmtime_r(&timeNow, &tm_now);

	message_pointer += sprintf(message_pointer, "{\"sensor_name\":\"%s\",", info->name());
	message_pointer += sprintf(message_pointer, "\"data_class_name\":\"%s\",", info->data_class_name());
	message_pointer += sprintf(message_pointer, "\"data\":{");
	message_pointer += sprintf(message_pointer, "\"at\":\"%d-%d-%d %02d:%02d:%02d +0000\",",
							   (tm_now.tm_year+1900), (tm_now.tm_mon+1), tm_now.tm_mday,
							   tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
	message_pointer += info->build(message_pointer, buff);
	message_pointer += sprintf(message_pointer, "}}");
	ESP_LOGI(TAG, "size = %d", strlen(q));
LEAVE_FUNC;
}

static	size_t
net_print(
	esp_http_client_handle_t	client,
	const char	*str)
{
	return	(esp_http_client_write(client, str, strlen(str)));
}

#define	net_printf(s, fmt, ...)		\
{					\
	char	buf[128];		\
	sprintf(buf, fmt, __VA_ARGS__);	\
	net_print(s, buf);		\
}

#ifdef	USE_WIFI
static	Bool
post_http(
	char	*host,
	char	*path,
	char	*session_key)
{
ENTER_FUNC;
	uint8_t	*pp;
	size_t	size;
	SensorInfo	*info;
	esp_http_client_handle_t	client;
	esp_err_t	ret;
	time_t	n;
	Bool	rc;
	int		status;

	rc = FALSE;
	ESP_LOGI(TAG, "buff %d bytes use", sense_buff->used_size());
	client = initialize_httpc();
  retry:
	sense_buff->rewind_read();
	if	( client != NULL )	{
		while	(  !sense_buff->is_end() )	{
			pp = sense_buff->mark_read();
			sense_buff->get(&n, sizeof(time_t));
			size = 0;
			size ++;	//	[
			while	( sense_buff->current_value() != 0xFF )	{
				message_pointer = Message;
				info = Sensors::item(sense_buff->get_value());
				make_one_sensor_message(n, info, sense_buff);
				size += strlen(Message);
				if	( sense_buff->current_value() != 0xFF )	{
					size ++;	//	,
				}
			}
			size ++;	//	]

			sense_buff->rewind_read(pp);
			httpc_set_header(client, "X-SESSION-KEY", session_key);
			httpc_set_header(client, "Content-Type", (char *)"application/json");
			ret = httpc_open(client, HTTP_METHOD_POST, host, path, size);
			if	( ret != ESP_OK )	{
				finish_httpc(client);
				goto	err;
			}
			sense_buff->get(&n, sizeof(time_t));
			net_print(client, "[");
			while	( sense_buff->current_value() != 0xFF )	{
				message_pointer = Message;
				info = Sensors::item(sense_buff->get_value());
				dbgprintf("info->id = %d", (int)info->id);
				make_one_sensor_message(n, info, sense_buff);
				ESP_LOGI(TAG, "Message:%s", Message);
				net_print(client, Message);
				if	( sense_buff->current_value() != 0xFF )	{
					net_print(client, ",");
				}
			}
			(void)sense_buff->get_value();
			net_print(client, "]");
			msleep(1000);
		}
		httpc_close(client);
		status = httpc_get_status(client);
		if	( status == 403 )	{
			api_relogin_device(client);
			msleep(100);
			goto retry;
		}
		finish_httpc(client);
		sense_buff->rewind_write();
		rc = TRUE;
	}
  err:;
LEAVE_FUNC;
	return	(rc);
}
#endif

extern	Bool
sensor_send_server(void)
{
	char	real_path[SIZE_PATH+1];
	Bool	rc = TRUE;
ENTER_FUNC;
#ifdef	USE_WIFI
	if	( !wifi_is_valid() )	{
		wifi_disconnect();
		wifi_connect(NULL, NULL);
	}
	if	( wifi_is_valid () )	{
		if		( sensor_url == NULL )	{
			sprintf(real_path, "%s/device/%s/data", CONSOLE_HOST, my_device_id);
		} else {
			strcpy(real_path, sensor_url);
		}
		rc = post_http(NULL, real_path, my_session_key);
		wifi_disconnect();
	} else {
		rc = FALSE;
	}
#endif
LEAVE_FUNC;
	return	(rc);
}

extern	Bool
sensor_collect(void)
{
	time_t	timeNow;

	time(&timeNow);

	Sensors::collect(timeNow, sense_buff);
	return	(TRUE);
}

extern	Bool
sensor_collect(
	SensorInfo	*info)
{
	time_t	timeNow;

ENTER_FUNC;
	time(&timeNow);

	Sensors::collect(timeNow, sense_buff, info);
LEAVE_FUNC;
	return	(TRUE);
}

extern	void
initialize_sensors_common(void)
{
#ifdef	USE_I2C
	initialize_i2c();
#endif

	sense_buff = new SenseBuffer(SIZE_OF_SENSOR_BUFF);	
}

extern	void
start_sensors(void)
{
	sense_buff->rewind_write();
	Sensors::start();
}
