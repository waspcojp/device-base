#define	TRACE

#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
extern	"C"	{
#include 	"config.h"
#include "nvs.h"
#include "nvs_flash.h"
#include 	"freertos/FreeRTOS.h"
#include 	"freertos/task.h"
#include	"freertos/event_groups.h"
#include 	"esp_system.h"
#include	"esp_http_server.h"
#include 	"esp_log.h"
#include	"time.h"
#include	"sys/time.h"
#include	"cJSON.h"
#include	"wifi.h"
#include	"httpc.h"
#include	"sntp.h"
#include	"api_client.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"
}
#include	"device.h"
#include	"setup_mode.h"

static	void
_wifi_scan(
	char	*buff)
{
	wifi_scan_start();
	wifi_scan_get(buff, NULL);
	dbgprintf("%s", buff);
}

static	Bool
_wifi_config(
	char	*body)
{
	cJSON	*root
		,	*node;
	Bool	ret = FALSE;
	
	dbgprintf("config: %s", body);
	root = cJSON_Parse(body);
	if ( root != NULL )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "ssid");
		if ( cJSON_IsString(node) )	{
			strcpy(ap_ssid, node->valuestring);
		} else {
			strcpy(ap_ssid, "** none **");
			ERROR("not string ssid");
		}
		node = cJSON_GetObjectItemCaseSensitive(root, "password");
		if	( cJSON_IsString(node) )	{
			strcpy(ap_pass, node->valuestring);
		} else {
			strcpy(ap_pass, "** none **");
			ERROR("not string pass");
		}
		dbgprintf("ssid: %s", ap_ssid);
		dbgprintf("pass: %s", ap_pass);
	} else {
		const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            dbgprintf("Error before: %s\n", error_ptr);
        }
	}
	cJSON_Delete(root);
	if		( wifi_connect(ap_ssid, ap_pass) )	{
		while	( !wifi_is_valid() )	{
			msleep(1000);
		}
		dbgmsg( "network is OK");
		open_device_info();
		set_ap_ssid(ap_ssid);
		set_ap_pass(ap_pass);
		commit_device_info();
		close_device_info();
		ret = TRUE;
	} else {
		dbgmsg( "network is NG");
		ret = FALSE;
	}
  error:;
	return	(ret);
}

static	Bool
_device_new(
	char	*body)
{
	cJSON	*root
		,	*node;
	static
		char	user[SIZE_USER + 1]
		,		pass[SIZE_PASS + 1]
		,		name[SIZE_NAME + 1];
	esp_http_client_handle_t	client;
	Bool	rc;

	dbgprintf("config: %s", body);
	*user = 0;
	*pass = 0;
	*my_device_id = 0;
	*my_session_key = 0;
	rc = FALSE;

	root = cJSON_Parse(body);
	if ( root != NULL )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "user");
		if ( cJSON_IsString(node) )	{
			strncpy(user, node->valuestring, SIZE_USER+1);
		}
		node = cJSON_GetObjectItemCaseSensitive(root, "password");
		if ( cJSON_IsString(node) )	{
			strncpy(pass, node->valuestring, SIZE_PASS+1);
		}
		node = cJSON_GetObjectItemCaseSensitive(root, "name");
		if ( cJSON_IsString(node) )	{
			strncpy(name, node->valuestring, SIZE_NAME+1);
		}
		node = cJSON_GetObjectItemCaseSensitive(root, "description");
		if		(	( *user != 0 )
				&&	( *pass != 0 ) )	{
			client = initialize_httpc();
			rc = api_device_new(client, user, pass, name, node->valuestring);
			if	( rc )	{
				rc = api_session_new(client, user, pass);
				if	( rc )	{
					open_device_info();
					set_device_id(my_device_id);
					set_session_key(my_session_key);
					commit_device_info();
					close_device_info();
				}
			}
			finish_httpc(client);
		}
		cJSON_Delete(root);
	}
	return	(rc);
}

static	Bool
_device_login(
	char	*body)
{
	cJSON	*root
		,	*node;
	static
		char	user[SIZE_USER + 1]
		,		pass[SIZE_PASS + 1]
		,		name[SIZE_NAME + 1];
	esp_http_client_handle_t	client;
	Bool	rc;

	dbgprintf("config: %s", body);
	*user = 0;
	*pass = 0;
	*my_device_id = 0;
	*my_session_key = 0;
	rc = FALSE;

	root = cJSON_Parse(body);
	if ( root != NULL )	{
		node = cJSON_GetObjectItemCaseSensitive(root, "user");
		if ( cJSON_IsString(node) )	{
			strncpy(user, node->valuestring, SIZE_USER+1);
		}
		node = cJSON_GetObjectItemCaseSensitive(root, "password");
		if ( cJSON_IsString(node) )	{
			strncpy(pass, node->valuestring, SIZE_PASS+1);
		}
		cJSON_Delete(root);
		if		(	( *user != 0 )
				&&	( *pass != 0 ) )	{
			client = initialize_httpc();
			rc = api_session_new(client, user, pass);
			if	( rc )	{
				open_device_info();
				set_device_id(my_device_id);
				set_session_key(my_session_key);
				commit_device_info();
				close_device_info();
			}
			finish_httpc(client);
		}
	}
	return	(rc);
}

#ifdef	API_TEST
static	void
initialize_setup_mode(void)
{
	_wifi_scan(ResponseMessage);
	_wifi_config("{\"ssid\":\"LANLANLAN24\",\"password\":\"8309f815520d2\"}");
	_device_new("{\"user\":\"ogochan\",\"password\":\"FOY600mg\",\"name\":\"ddd\",\"description\":\"\"}");
}
#else
static	void
httpd_resp(
	httpd_req_t *req,
	char        *body,
	char        *code)
{
ENTER_FUNC;
	httpd_resp_set_hdr(req,
		"Access-Control-Allow-Origin","*");
	if	( code )	{
		httpd_resp_set_status(req, code);
	}
    httpd_resp_send(req, body, strlen(body));
LEAVE_FUNC;
}

static	esp_err_t
http_wifi_scan(
	httpd_req_t	*req)
{
ENTER_FUNC;
	_wifi_scan(ResponseMessage);
    httpd_resp(req, ResponseMessage, NULL);
LEAVE_FUNC;
    return ESP_OK;
}

static	esp_err_t
http_wifi_config(
	httpd_req_t	*req)
{
ENTER_FUNC;
	httpd_req_recv(req, ResponseMessage, req->content_len);
	ResponseMessage[req->content_len] = 0;
	if	( _wifi_config(ResponseMessage) )	{
    	httpd_resp(req, "OK", NULL);
	} else {
    	httpd_resp(req, "NG", "405");
	}
LEAVE_FUNC;
    return ESP_OK;
}

static	esp_err_t
http_device_new(
	httpd_req_t	*req)
{
ENTER_FUNC;
	httpd_req_recv(req, ResponseMessage, req->content_len);
	ResponseMessage[req->content_len] = 0;
	if	( _device_new(ResponseMessage) )	{
		httpd_resp(req, "OK", NULL);
	} else {
		httpd_resp(req, "NG", "405");
	}
LEAVE_FUNC;
    return ESP_OK;
}

static	esp_err_t
http_device_login(
	httpd_req_t	*req)
{
ENTER_FUNC;
	httpd_req_recv(req, ResponseMessage, req->content_len);
	ResponseMessage[req->content_len] = 0;
	if	( _device_login(ResponseMessage) )	{
		httpd_resp(req, "OK", NULL);
	} else {
		httpd_resp(req, "NG", "405");
	}
LEAVE_FUNC;
    return ESP_OK;
}

static	esp_err_t
http_options(
	httpd_req_t	*req)
{
	httpd_resp_set_hdr(req,
		"Access-Control-Allow-Origin","*");
	httpd_resp_set_hdr(req,
		"Access-Control-Allow-Method","POST, GET, PUT, DELETE, OPTIONS");
	httpd_resp_set_hdr(req,
		"Access-Control-Allow-Headers","X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept");
	httpd_resp_set_hdr(req,
		"Access-Control-Allow-max-Age","86400");
    httpd_resp_send(req, "OK", 3);
	return	ESP_OK;
}

static	esp_err_t
http_device_reset(
	httpd_req_t	*req)
{
	httpd_req_recv(req, ResponseMessage, req->content_len);
	ResponseMessage[req->content_len] = 0;
	httpd_resp(req, "OK", NULL);
	msleep(1000);
	esp_restart();
    return ESP_OK;
}

static	const	httpd_uri_t	handlers[] = {
    {
		.uri      = "*",
    	.method   = HTTP_OPTIONS,
    	.handler  = http_options,
    	.user_ctx = NULL,
    },
    { 
		.uri      = "/wifi/config",
    	.method   = HTTP_POST,
    	.handler  = http_wifi_config,
    	.user_ctx = NULL,
    },
    {
		.uri      = "/wifi/scan",
		.method   = HTTP_GET,
		.handler  = http_wifi_scan,
		.user_ctx = NULL,
    },
    { .uri      = "/device",
      .method   = HTTP_POST,
      .handler  = http_device_new,
      .user_ctx = NULL,
    },
    { .uri      = "/device/login",
      .method   = HTTP_POST,
      .handler  = http_device_login,
      .user_ctx = NULL,
    },
    { .uri      = "/device/reset",
      .method   = HTTP_POST,
      .handler  = http_device_reset,
      .user_ctx = NULL,
    },
};

static	void
initialize_setup_mode(void)
{
    httpd_handle_t	hd;
    httpd_config_t	config = HTTPD_DEFAULT_CONFIG();
    int		i;

    config.server_port = SERVER_PORT;
    config.max_open_sockets = 2;
	config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&hd, &config) == ESP_OK) {
        dbgprintf("Started HTTP server on port: '%d'", config.server_port);
        dbgprintf("Max URI handlers: '%d'", config.max_uri_handlers);
        dbgprintf("Max Open Sessions: '%d'", config.max_open_sockets);
        dbgprintf("Max Header Length: '%d'", HTTPD_MAX_REQ_HDR_LEN);
        dbgprintf("Max URI Length: '%d'", HTTPD_MAX_URI_LEN);
        dbgprintf("Max Stack Size: '%d'", config.stack_size);

		for (i = 0; i < sizeof(handlers)/sizeof(httpd_uri_t); i++) {
			dbgprintf("method = %d path %s", handlers[i].method, handlers[i].uri);
			if	( httpd_register_uri_handler(hd, &handlers[i]) != ESP_OK )	{
				dbgprintf("register uri failed for %d", i);
				return;
			}
		}
	}

}
#endif

extern	void
load_device_info(void)
{
ENTER_FUNC;
	open_device_info();

	get_device_id(my_device_id);
	get_session_key(my_session_key);
	get_my_ssid(my_ssid);
	get_my_pass(my_pass);
	get_ap_ssid(ap_ssid);
	get_ap_pass(ap_pass);
	close_device_info();
	dbgprintf("ap ssid    : %s", ap_ssid);
	dbgprintf("ap pass    : %s", ap_pass);
	dbgprintf("device uuid: %s", my_device_id);
	dbgprintf("session key: %s", my_session_key);
LEAVE_FUNC;
}

extern	void
destroy_device_info(void)
{
	memclear(my_ssid, SIZE_SSID + 1);
	memclear(my_pass, SIZE_PASS + 1);
	memclear(my_device_id, SIZE_UUID + 1);
	memclear(my_session_key, SIZE_UUID + 1);
	memclear(ap_ssid, SIZE_SSID + 1);
	memclear(ap_pass, SIZE_PASS + 1);

	strcpy(my_ssid, DEFAULT_SSID);
	strcpy(my_pass, DEFAULT_PASS);

	save_device_info();
}

extern	void
save_device_info(void)
{
	open_device_info();

	set_my_ssid(my_ssid);
	set_my_pass(my_pass);
	set_device_id(my_device_id);
	set_session_key(my_session_key);
	set_ap_ssid(ap_ssid);
	set_ap_pass(ap_pass);

	close_device_info();
}

extern	void
SetupMode(void)
{
ENTER_FUNC;
	show_status(STATUS_SETUP);
	wifi_ap_start(my_ssid, my_pass);
	initialize_setup_mode();
LEAVE_FUNC;
}

