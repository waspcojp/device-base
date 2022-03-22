#define	TRACE

#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include 	"config.h"
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include 	<esp_system.h>
#include	<esp_tls.h>
#include 	<esp_log.h>
#include	<time.h>
#include	<sys/time.h>
#include	"cJSON.h"
#include	"esp_http_client.h"
#include	"httpc.h"
#include	"nvs.h"
#include	"api_client.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"

#ifdef	USE_LOCALCA
extern	const	unsigned	char	ca_key_start[]		asm("_binary_ca_crt_start");
extern	const	unsigned	char	ca_key_end[]		asm("_binary_ca_crt_end");
#endif

extern	void
initialize_api(void)
{
#ifdef	USE_LOCALCA
	esp_tls_init_global_ca_store();
	esp_tls_set_global_ca_store(ca_key_start, ca_key_end - ca_key_start);
#endif
}

extern	Bool
api_device_new(
	esp_http_client_handle_t	client,
	char	*user,
	char	*pass,
	char	*name,
	char	*description)
{
	Bool	rc;
	cJSON	*root
		,	*node;
	int		size;

ENTER_FUNC;
	rc = FALSE;
	dbgprintf("%s:%s:%s:%s", user,pass, DEVICE_CLASS, name);
	sprintf(Message, 
		"{\"user\":\"%s\",\"password\":\"%s\",\"name\":\"%s\",\"class\":\"%s\",\"description\":\"%s\"}",
		user, pass, name, DEVICE_CLASS,
		description);
	if	( httpc_post(client, HTTP_METHOD_POST,
			CONSOLE_HOST, "/devices", (uint8_t *)Message, strlen(Message)) != ESP_OK )	goto	err;
	// int status =  httpc_get_status(client);
	//dbgprintf("status: %d", status);
	memset(ResponseMessage, 0, SIZE_RESPONSE_BUFFER);
	if	( ( size = httpc_get_body(client, (uint8_t *)ResponseMessage, SIZE_RESPONSE_BUFFER) ) < 0 )	goto	err;
	dbgprintf("get(%d): %s", size, ResponseMessage);
	//if		( status != 200 )	goto	err;
	root = cJSON_Parse(ResponseMessage);
	if	( root )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "code");
		if 		(	( cJSON_IsNumber(node) )
				&&	( node->valueint == 0  ))	{
			node = cJSON_GetObjectItemCaseSensitive(root, "uuid");
			if	( cJSON_IsString(node) )	{
				strncpy(my_device_id, node->valuestring, SIZE_UUID+1);
				dbgprintf("device uuid: %s", my_device_id);
				rc = TRUE;
			}
		}
		cJSON_Delete(root);
	}
  err:;
	httpc_close(client);
LEAVE_FUNC;
	return	(rc);
}

extern	Bool
api_session_new(
	esp_http_client_handle_t	client,
	char	*user,
	char	*pass)
{
	Bool	rc;
	cJSON	*root
		,	*node;
	int		size
		,	status;


ENTER_FUNC;
	rc = FALSE;
	if		(	( user != NULL )
			&&	( pass != NULL ) )	{
		sprintf(Message, "{\"user\":\"%s\",\"password\":\"%s\",\"device\":\"%s\"}", user, pass, my_device_id);
	} else {
		sprintf(Message, "{\"device\":\"%s\"}", my_device_id);
	}
	dbgprintf("Message: %s", Message);
	if ( httpc_post(client, HTTP_METHOD_POST, CONSOLE_HOST, "/session", (uint8_t *)Message, strlen(Message)) != ESP_OK )	goto	err;
	status =  httpc_get_status(client);
	dbgprintf("status: %d", status);
	if	( status != 200 )	goto	err;
	if	( ( size = httpc_get_body(client, (uint8_t *)ResponseMessage, SIZE_RESPONSE_BUFFER) ) < 0 )	goto	err;
	dbgprintf("get: %s", ResponseMessage);
	root = cJSON_Parse(ResponseMessage);
	if	( root )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "code");
		if 		(	( cJSON_IsNumber(node) )
				&&	( node->valueint == 0  ))	{
			node = cJSON_GetObjectItemCaseSensitive(root, "session_key");
			if		(	( node )
					&&	( cJSON_IsString(node) ) )	{
				strncpy(my_session_key, node->valuestring, SIZE_UUID+1);
				dbgprintf("session key: %s", my_session_key);
				rc = TRUE;
			}
		}
		cJSON_Delete(root);
	}
  err:;
	httpc_close(client);
LEAVE_FUNC;
	return	(rc);
}

extern	Bool
api_post_data(
	esp_http_client_handle_t	client,
	esp_http_client_method_t	method,
	const	char	*path,
	uint8_t	*data,
	size_t	size)
{
	Bool	rc;
	cJSON	*root
		,	*node;
	int		status;

	char	real_path[SIZE_PATH+1];

ENTER_FUNC;
	rc = FALSE;
	sprintf(real_path, "/device/%s%s", my_device_id, path);
	httpc_set_header(client, "X-SESSION-KEY", my_session_key);
	if ( httpc_post(client, method, CONSOLE_HOST, real_path, data, size) != ESP_OK )	goto	err;
	status =  httpc_get_status(client);
	dbgprintf("status: %d", status);
	if	( status != 201 )	goto	err;
	if	( httpc_get_body(client, (uint8_t *)ResponseMessage, SIZE_RESPONSE_BUFFER) < 0 )	goto	err;
	dbgprintf("get: %s", ResponseMessage);
	root = cJSON_Parse(ResponseMessage);
	if	( root )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "code");
		if 		(	( cJSON_IsNumber(node) )
				&&	( node->valueint == 0  ))	{
			dbgmsg("OK");
			rc = TRUE;
		}
		cJSON_Delete(root);
	}
  err:;
	httpc_close(client);
LEAVE_FUNC;
	return	(rc);
}

extern	int
api_get_data(
	esp_http_client_handle_t	client,
	const	char	*path,
	uint8_t	*data,
	size_t	size,
	int		*status)
{
	int		rc
	,		ret;
	int		_status;

	static	char	real_path[SIZE_PATH+1];

ENTER_FUNC;
	rc = -1;
	_status = 0;
dbgprintf("session: %s", my_session_key);
	sprintf(real_path, "/device/%s%s", my_device_id, path);
	esp_http_client_set_header(client, "X-SESSION-KEY", my_session_key);

	ret = httpc_open(client, HTTP_METHOD_GET, CONSOLE_HOST, real_path, 0);
dbgprintf("ret = %d", ret);
	if ( ret != ESP_OK )	{
		rc = -1;
		goto	err;
	}
	rc = httpc_get_data(client, data, size);
dbgprintf("rc = %d", rc);

	_status =  httpc_get_status(client);
#ifdef	TRACE
	char	*p;
	int		c;
	p = ResponseMessage;
	while	( strlen(p) > 40 ) {
		c = *(p + 40);
		*(p + 40) = 0;
		ESP_LOGI("Recv: ", "%s", p);
		*(p + 40) = c;
		p += 40;
	}
	ESP_LOGI("Recv: ", "%s", p);
#endif

  err:;
	httpc_close(client);
LEAVE_FUNC;
	if	( status != NULL )	{
		*status = _status;
	}
	dbgprintf("status: %d", _status);
	return	(rc);
}

extern	Bool
api_exec_ota(void)
{
	char	real_path[SIZE_PATH+1];
	Bool	ret;

ENTER_FUNC;
	sprintf(real_path, "/device/%s/farm", my_device_id);
	ret = httpc_ota(CONSOLE_HOST, real_path, my_session_key);
LEAVE_FUNC;
	return	(ret);
}

extern	void
api_relogin_device(
	esp_http_client_handle_t	client)
{
	if	( api_session_new(client, NULL, NULL) )	{
		open_device_info();
		set_session_key(my_session_key);
		close_device_info();
	}
}