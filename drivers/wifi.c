#define	TRACE

#include 	<stdio.h>
#include	<sys/time.h>
#include	<string.h>

#include 	"freertos/FreeRTOS.h"
#include 	"freertos/task.h"
#include	"freertos/event_groups.h"

#include 	"esp_system.h"
#include	"esp_wifi.h"
#include	"esp_event.h"
//#include	"esp_event_loop.h"
#include 	"esp_log.h"
#include	"time.h"

#include	"lwip/err.h"
#include	"lwip/sys.h"

#include 	"config.h"
#include	"wifi.h"
#include	"types.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"

#define	NUM_APS				32

#define	WIFI_CONNECTED_BIT	BIT0
#define WIFI_FAIL_BIT       BIT1

static	EventGroupHandle_t	s_wifi_event_group;
static	int					s_retry_num = 0;
static	wifi_ap_record_t	aps[NUM_APS];
static	Bool	wifi_started;
static	Bool	wifi_connected;
static	esp_netif_t *if_ap;
static	esp_netif_t *if_sta;
static	wifi_config_t wifi_config;

static	esp_err_t
event_handler(
	void* arg,
	esp_event_base_t event_base, 
	int32_t event_id,
	void* event_data)
{
	if	(  event_base == WIFI_EVENT )	{
		ip_event_got_ip_t	*event = (ip_event_got_ip_t*)event_data;
		wifi_event_ap_staconnected_t *wifi_event = (wifi_event_ap_staconnected_t *) event_data;

		switch(event_id) {
	  	  case SYSTEM_EVENT_STA_START:
        	esp_wifi_connect();
    		break;
		  case WIFI_EVENT_AP_START:
			dbgmsg("AP start");
			break;
		  case SYSTEM_EVENT_STA_DISCONNECTED:
			if	(  s_retry_num < MAX_RETRY )	{
				esp_wifi_connect();
				s_retry_num ++;
				dbgmsg("retry to connect to AP");
			} else {
				xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
				dbgmsg("connect to the AP fail\n");
			}
			break;
		  case	WIFI_EVENT_AP_STACONNECTED:
        	dbgprintf( "station "MACSTR" join, AID=%d",
            	     MAC2STR(wifi_event->mac), wifi_event->aid);
			break;
		  case	WIFI_EVENT_AP_STADISCONNECTED:
        	dbgprintf( "station "MACSTR" leave, AID=%d",
            	     MAC2STR(wifi_event->mac), wifi_event->aid);
			break;
		  default:
        	break;
    	}
	}
	if	( event_base ==  IP_EVENT ) {
		ip_event_got_ip_t	*event = (ip_event_got_ip_t*)event_data;
		wifi_event_ap_staconnected_t *wifi_event = (wifi_event_ap_staconnected_t *) event_data;
		switch(event_id) {
		  case IP_EVENT_STA_GOT_IP:
 	    	dbgprintf( "got ip:%s",
                		ip4addr_ntoa(&event->ip_info.ip));
 			wifi_connected = TRUE;
			s_retry_num = 0;
			xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        	break;
		  default:
			dbgprintf("event %d", event_id);
        	break;
    	}
	}
    return ESP_OK;
}

static	esp_event_handler_instance_t instance_any_id;
static	esp_event_handler_instance_t instance_got_ip;

extern	esp_err_t
initialize_wifi(void)
{
	esp_err_t err;
    esp_netif_ip_info_t ip = {
        .ip = { .addr = ipaddr_addr(SERVER_ADDRESS) },
        .gw = { .addr = ipaddr_addr(SERVER_GATEWAY) },
        .netmask = { .addr = ipaddr_addr(SERVER_NETMASK) },
	};
	esp_netif_inherent_config_t	base = {
	        .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_AUTOUP),
        	ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac)
        	.ip_info = &ip,
        	.get_ip_event = ESP_NETIF_IP_EVENT_GOT_IP,
        	.lost_ip_event = ESP_NETIF_IP_EVENT_LOST_IP,
        	.if_key = "WIFI_AP_NEW",
        	.if_desc = "ap",
        	.route_prio = 10
		};
ENTER_FUNC;
	s_wifi_event_group = xEventGroupCreate();
	ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
#if	0
	if_ap = esp_netif_create_default_wifi_ap();
#else
	{
	    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
		cfg.base = &base;
    	if_ap = esp_netif_new(&cfg);
    	esp_netif_attach_wifi_ap(if_ap);
    	esp_wifi_set_default_wifi_ap_handlers();
	}
#endif

	if_sta = esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ERROR_CHECK( esp_wifi_init(&cfg) );

    ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &event_handler,
                                                    if_ap,
                                                    &instance_any_id));
    ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                    IP_EVENT_STA_GOT_IP,
                                                    &event_handler,
                                                    if_sta,
                                                    &instance_got_ip));


	ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));

	wifi_started = FALSE;
	wifi_connected = FALSE;
  err_exit:
LEAVE_FUNC;
    return	(err);
}

extern	void
wifi_ap_start(
	char	*ssid,
	char	*pass)
{
	int	rc;
    wifi_config_t wifi_config = {
        .ap = {
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
	strcpy((char *)wifi_config.ap.ssid, ssid);
	wifi_config.ap.ssid_len = strlen(ssid);
	strcpy((char *)wifi_config.ap.password, pass);

    if	( strlen(pass) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    //ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    dbgprintf( "wifi_init_softap finished. SSID:%s password:%s",
             my_ssid, my_pass);
	wifi_started = TRUE;
}

extern	void
wifi_scan_start(void)
{
    ESP_ERROR_CHECK( esp_wifi_start() );
}

extern	void
wifi_scan_get(
	char	*p,
	const char	*format)
{
	wifi_scan_config_t	scan_config = {
		.ssid = NULL,
		.bssid = NULL,
		.channel = 0,
		.show_hidden = 0,
		.scan_type = WIFI_SCAN_TYPE_ACTIVE,
	};
	uint16_t	num;
	uint16_t	gnum;

	ESP_ERROR_CHECK( esp_wifi_scan_start(&scan_config, true) );
	ESP_ERROR_CHECK( esp_wifi_scan_get_ap_num(&num) );
	dbgprintf( "AP number = %d", (int)num);
	gnum = NUM_APS;
	ESP_ERROR_CHECK( esp_wifi_scan_get_ap_records(&gnum, aps) );
	dbgprintf( "AP get = %d", (int)gnum);
	if	( format == NULL )	{
		*p ++ = '[';
		for	( int i = 0 ; i < gnum; i ++ )	{
			if ( i > 0 )	{
				*p ++ = ',';
			}
			p += sprintf(p, "{\"ssid\": \"%s\", \"rssi\": %d}", aps[i].ssid, (int)aps[i].rssi);
			dbgprintf( "SSID: %s rssi %d", aps[i].ssid, (int)aps[i].rssi);
		}
		*p ++ = ']';
		*p = 0;
	} else {
		for	( int i = 0 ; i < gnum; i ++ )	{
			p += sprintf(p, format, aps[i].ssid, (int)aps[i].rssi);
		}
	}
	ESP_ERROR_CHECK( esp_wifi_scan_stop() );
}

extern	Bool
wifi_connect(
	char	*ssid,
	char	*password)
{
	Bool	rc;
ENTER_FUNC;
	strcpy((char *)wifi_config.sta.ssid, ssid);
	strcpy((char *)wifi_config.sta.password, password);

	rc = FALSE;
	if	( !wifi_connected ) {
		if	( !wifi_started )	{
			ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
			ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );

	    	ESP_ERROR_CHECK(esp_wifi_start());
		} else {
			ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
			ESP_ERROR_CHECK(esp_wifi_connect());
		}
		dbgmsg("*");
		EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
    			WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    			pdFALSE,
    			pdFALSE,
				( 1000 * 30 ) / portTICK_PERIOD_MS);
    			//portMAX_DELAY);
		dbgmsg("*");
		if	( bits & WIFI_CONNECTED_BIT )	{
    		dbgprintf("connected to ap SSID:%s password:%s", ssid, password);
			rc = TRUE;
			wifi_started = TRUE;
    	} else
		if	( bits & WIFI_FAIL_BIT )	{
    		dbgprintf("Failed to connect to SSID:%s, password:%s", ssid, password);
    	} else {
        	ESP_LOGE("", "UNEXPECTED EVENT");
    	}

    	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
		dbgmsg("*");
    	vEventGroupDelete(s_wifi_event_group);
	} else {
		rc = TRUE;
	}
LEAVE_FUNC;
	return(rc);
}

extern	Bool
wifi_is_valid(void)
{
	return	(wifi_started);
}
extern	void
wifi_disconnect(void)
{
	if	( wifi_connected )	{
		ESP_ERROR_CHECK( esp_wifi_stop() );
		ESP_ERROR_CHECK( esp_wifi_deinit() );
	    ESP_ERROR_CHECK(esp_event_loop_delete_default());
		esp_netif_destroy(if_ap);
		esp_netif_destroy(if_sta);
		wifi_connected = FALSE;
		wifi_started = FALSE;
	}
}
extern	void
wifi_stop(void)
{
ENTER_FUNC;
	if	( wifi_started )	{
		ESP_ERROR_CHECK( esp_wifi_stop() );
		wifi_started = FALSE;
	}
LEAVE_FUNC;
}
