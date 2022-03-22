#define TRACE

#include    "config.h"
#ifdef  USE_COMMAND

extern  "C" {
#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include 	<esp_log.h>

#include    "nvs.h"
#include	"wifi.h"
#include	"types.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"
}
#include    "UART2.h"
#include    "command.h"
#include    "command_wifi.h"
#include	"utility.h"

#ifdef  USE_WIFI

static  void
exec_wifi_save(void)
{
	open_device_info();

	set_ap_ssid(ap_ssid);
	set_ap_pass(ap_pass);

	close_device_info();
}

static  void
exec_wifi_connect(void)
{
    esp_restart();
}

static  void
exec_wifi_disconnect(void)
{
    wifi_disconnect();
}

static  void
exec_wifi_status(void)
{
    uprintf("STATUS: %s\n", ( wifi_is_valid() ) ? "CONNECTED" : "DISCONNECTED");
    uprintf("SSID: %s\n", ap_ssid);
    uprintf("PASS: %s\n", ap_pass);
}
static  void
exec_wifi_scan(void)
{
    char    buff[2048];
    wifi_scan_start();
    wifi_scan_get(buff, "%s\t%d\n");
    uart2_puts(buff); uart2_puts("\n");
}
extern  void
exec_wifi(void)
{
    int     token;
    char    *arg;

    if  (   ( ( token = get_token() ) == TOKEN_SYMBOL )
        ||  ( token == TOKEN_STRING ) ) {
        arg = get_word();
        if  ( !strcasecmp(arg, "SSID") ) {
            token = get_token();
            if  (   ( token == TOKEN_STRING )
                ||  ( token == TOKEN_SYMBOL ) ) {
                strcpy(ap_ssid, get_word());
                (void)get_token();
            } else {
                skip_statement("SSID invalid");
            }
        } else
        if  ( !strcasecmp(arg, "PASS" ) )   {
            token = get_token();
            if  (   ( token == TOKEN_STRING )
                ||  ( token == TOKEN_SYMBOL ) ) {
                strcpy(ap_pass, get_word());
                (void)get_token();
            } else {
                skip_statement("PASS invalid");
            }
        } else
        if  ( !strcasecmp(arg, "STATUS" ) ) {
            exec_wifi_status();
            (void)get_token();
        } else
        if  ( !strcasecmp(arg, "SCAN") )    {
            exec_wifi_scan();
        } else
#if 1       //  broken
        if  ( !strcasecmp(arg, "CONNECT") )   {
            exec_wifi_connect();
            (void)get_token();
        } else
#endif
        if  ( !strcasecmp(arg, "DISCONNECT") )   {
            exec_wifi_disconnect();
            (void)get_token();
        } else
        if  ( !strcasecmp(arg, "save" ) )   {
            exec_wifi_save();
            (void)get_token();
        } else {
            skip_statement("invalid sub command");
        }
    } else {
        skip_statement();
    }
}

#endif  //  USE_WIFI
#endif  //  USE_COMMAND
