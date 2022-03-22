#define TRACE

#include    "config.h"
#ifdef  USE_COMMAND

extern  "C" {
#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include    <math.h>
#include 	<freertos/FreeRTOS.h>
#include 	<freertos/task.h>
#include 	<esp_log.h>
#include	<time.h>

#include    "nvs.h"
#include	"httpc.h"
#include    "schedule.h"
#include    "api_client.h"
#include	"types.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"
}
#include    "setup_mode.h"
#include	"SensorInfo.h"
#include	"Sensors.h"
#include    "UART2.h"
#include    "command.h"
#include	"utility.h"
#ifdef TESTCODE
#include    "testcode.h"
#endif

extern  "C++" {
static  void
list(
    int index,
    SensorInfo *info)
{
    uprintf( "%d:%s\n", index, info->name());
}
    
static  void
show_sensor(
    SensorInfo  *info)
{
    uprintf("name: %s\n", info->name());
    uprintf("class: %s\n", info->data_class_name());
    uprintf("value: %f %s\n", info->value(), info->unit());
    uart2_puts("\n");
}

extern  void
exec_sensor(void)
{
    int token;
    char    *arg;

ENTER_FUNC;
    token = get_token();
    if      (   ( token == TOKEN_SYMBOL )
            ||  ( token == TOKEN_STRING ))  {
        arg = get_word();
        if  ( !strcasecmp(arg, "list") )    {
            Sensors::iter(list);
            (void)get_token();
        } else {
            SensorInfo  *info = Sensors::find(arg);
            if      ( info )    {
                show_sensor(info);
                (void)get_token();
            } else {
                skip_statement("sensor not found");
            }
        }
    } else
    if      ( token == TOKEN_NUMBER )   {
        int no = get_int();
        SensorInfo  *info = Sensors::item(no);
        show_sensor(info);
        (void)get_token();
    } else {
        skip_statement();
    }
LEAVE_FUNC;
}

extern  void
exec_schedule_control(void)
{
    char    *arg;
    int     token;

    if  (   ( ( token = get_token() ) == TOKEN_SYMBOL )
        ||  ( token == TOKEN_STRING ) ) {
        arg = get_word();
        if  ( !strcasecmp(arg, "ON") )  {
            start_schedule();
            uart2_puts("schedule procedure started\n");
        } else
        if  ( !strcasecmp(arg, "OFF") ) {
            stop_schedule();
            uart2_puts("schedule procedure stopped\n");
        }
        (void)get_token();
    } else {
        skip_statement();
    }
}

extern  void
exec_reset(void)
{
    dbgmsg("reset");
    esp_restart();
}

extern  void
exec_device(void)
{
    char    *arg;
    int     token;
    char    user[SIZE_USER+1]
        ,   pass[SIZE_PASS+1]
        ,   name[SIZE_NAME+1];

    if  ( ( token = get_token() ) == TOKEN_SYMBOL )   {
        arg = get_word();
        if  ( !strcasecmp(arg, "new") )    {
            token = get_token();
            if  (   ( token == TOKEN_SYMBOL )
                ||  ( token == TOKEN_STRING ))  {
                strcpy(user, get_word());
            } else
                goto    error;
            if  ( ( token = get_token() ) != ':' )    goto    error;
            token = get_token();
            if  (   ( token == TOKEN_SYMBOL )
                ||  ( token == TOKEN_STRING ))  {
                strcpy(pass, get_word());
            } else
                goto    error;
            token = get_token();
            switch  (token) {
              case  ':':
                token = get_token();
                if  (   ( token == TOKEN_SYMBOL )
                    ||  ( token == TOKEN_STRING ))  {
                    strcpy(name, get_word());
                } else
                    goto    error;
                break;
              case  TOKEN_EMPTY:
                name[0] = 0;
                break;
              default:
                goto    error;
            }
            uprintf("user: %s\npass: %s\nname: %s\n", user, pass, name);
		    if		(	( *user != 0 )
			    	&&	( *pass != 0 ) )	{
			    esp_http_client_handle_t    client = initialize_httpc();
	            *my_device_id = 0;
	            *my_session_key = 0;
			    Bool    rc = api_device_new(client, user, pass, name, "");
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
            skip_statement("OK");
        } else
        if  ( !strcasecmp(arg, "login") )    {
            token = get_token();
            if  (   ( token == TOKEN_SYMBOL )
                ||  ( token == TOKEN_STRING ))  {
                strcpy(user, get_word());
            } else
                goto    error;
            if  ( ( token = get_token() ) != ':' )    goto    error;
            token = get_token();
            if  (   ( token == TOKEN_SYMBOL )
                ||  ( token == TOKEN_STRING ))  {
                strcpy(pass, get_word());
            } else
                goto    error;
            uprintf("user: %s\npass: %s\n", user, pass);
		    if		(	( *user != 0 )
			    	&&	( *pass != 0 ) )	{
			    esp_http_client_handle_t    client = initialize_httpc();
	            *my_session_key = 0;
				Bool    rc = api_session_new(client, user, pass);
				if	( rc )	{
					open_device_info();
					set_session_key(my_session_key);
					commit_device_info();
					close_device_info();
                    uart2_puts("login OK\n");
				} else {
                    uart2_puts("login fail.\n");
                }
			    finish_httpc(client);
            }
            skip_statement();
        } else
        if  ( !strcasecmp(arg, "info") )    {
            uprintf("timezone %s\n",getenv("TZ"));
            uprintf("device-id: %s\nsession-key: %s", my_device_id, my_session_key);
            skip_statement("OK");
        } else
        if  ( !strcasecmp(arg, "clear") )   {
            destroy_device_info();
            skip_statement("OK");
        } else
        if  ( !strcasecmp(arg, "reset") )   {
            exec_reset();
#ifdef  USE_OTA
        } else
        if  ( !strcasecmp(arg, "ota") ) {
            if  ( api_exec_ota() )  {
                skip_statement("OK");
            } else {
                skip_statement("fail");
            }
#endif
        } else {
            skip_statement("invalid operation");
        }
    } else {
error:
        skip_statement("syntax error");
    }
}

extern  Command_t   Command[];

static  void
exec_help(void)
{
    int     token = get_token();

    if  (   ( token == TOKEN_EMPTY )
        ||  ( token == ';'         ))   {
        for ( int i = 0; *Command[i].command != 0; i ++)    {
            uprintf("%s\n", Command[i].command);
        }
    } else {
        char    *q = get_word();
        for ( int i = 0; *Command[i].command != 0; i ++)    {
            if  ( !strcasecmp(q, Command[i].command) )  {
                if  ( Command[i].help != NULL ) {
                    uprintf("%s\n", Command[i].help);
                }
                break;
            }
        }
    }
    skip_statement();
}

/*
 *   Command parser
 */

static  char    *input;
static  double  number;
static  char    word_buff[LINE_LENGTH+1];

extern  char    *
get_word(void)
{
    return  (word_buff);
}

extern  double
get_float(void)
{
    return  (number);
}
extern  int
get_int(void)
{
    return  ((int)round(number));
}

extern  void
set_input(
    char    *s)
{
    input = s;
}

extern  int
get_token(void)
{
    int     token;
    char    *word;

    while   (   ( *input != 0 )
            &&  (   ( isspace(*input) )
                ||  ( iscntrl(*input) )))
         input ++;

    switch   (*input)    {
      case  0:
        token = TOKEN_EMPTY;
        dbgmsg("EMPTY");
        break;
      case  '"':
        input ++;
        word = word_buff;
        while   (   ( *input != 0 )
                &&  ( *input != '"' ))  {
            *word ++ = *input ++;
        }
        *word = 0;
        token = TOKEN_STRING;
        input ++;
        break;
        dbgprintf("STRING:%s", word_buff);
      case  ';':
      case  ':':
        token = *input;
        input ++;
        dbgprintf("CHAR %c", token);
        break;
      default:
        if  (   ( isdigit(*input) )
            ||  ( *input == '.'   )
            ||  ( *input == '-'   ))    {
            word = word_buff;
            while   (   ( *input != 0 )
                    &&  (   ( isdigit(*input) )
                        ||  ( *input == '-'   )
                        ||  ( *input == '.'   ))) {
                *word ++ = *input ++;
            }
            *word = 0;
            number = atof(word_buff);
            token = TOKEN_NUMBER;
            dbgprintf("NUMBER:%s", word_buff);
        } else {
            word = word_buff;
            while   (   ( *input != 0 )
                    &&  (   ( isalnum(*input) )
                        ||  ( *input == '_'   ) ))
                *word ++ = *input ++;
            *word = 0;
            token = TOKEN_SYMBOL;
            dbgprintf("SYMBOL:%s", word_buff);
        }
        break;
    }
    return  (token);
}

extern  void
skip_statement(void)
{
    int     token;
    while   (   ( ( token = get_token() ) != TOKEN_EMPTY )
            &&  ( token != ';' ));
}

extern  void
skip_statement(
    const   char    *msg)
{
    uart2_puts(msg);
    uart2_puts("\n");
    skip_statement();
}

static  void
parse_command(void)
{
    char    *q;
    int     token;

    token = get_token();
    while   ( token != TOKEN_EMPTY )  {
        if  ( token == TOKEN_SYMBOL ) {
            q = get_word();
            dbgprintf("predicate %s", q);
            if  ( !strcasecmp(q, "help") )  {
                exec_help();
            } else
            for ( int i = 0; *Command[i].command != 0; i ++)    {
                if  ( !strcasecmp(q, Command[i].command) )  {
                    (*Command[i].func)();
                    break;
                }
            }
        } else {
            skip_statement("command not found.");
        }
        token = get_token();
    }
}

extern  size_t
read_input(
    const   char    *prompt,
    char    *buff,
    size_t  length)
{
    uint8_t c;
    size_t  got;
    char    *q = buff;

    uart2_puts(prompt);
    while   ((buff - q) < length)  {
        got = uart2_read(&c, 1);
        if ( got == 1 ) {
            switch  (c) {
              case  0x1B:
                uart2_puts("<ESC>\r\n");
                *q = c;
                buff = q + 1;
                goto    quit;
              case  '\n':
              case  '\r':
                uart2_puts("\n\r\n");
                *buff = 0;
                goto    quit;
              case  0x08:   //  BS
                uprintf("%c %c", c,c);
                buff --;
                *buff = 0;
                break;
              default:
                uprintf("%c", c);
                *buff ++ = c;
                break;
            }
        }
    }
  quit:
    *buff = 0;
    return  (buff - q);
}

static  void
command_loop(
    void    *args)
{
    char    buff[LINE_LENGTH+1];

    while   (TRUE)  {
        (void)read_input("> ", buff, LINE_LENGTH);
        dbgprintf("input:%s", buff);
        set_input(buff);
        parse_command();
    }

}
}

extern  "C" {
extern  void
start_command(void)
{
ENTER_FUNC;
    uart2_init();
	(void)xTaskCreate(command_loop, "command_loop", 8192, NULL, 5, NULL);
#ifdef TESTCODE
    testcode();
#endif
LEAVE_FUNC;
}
}
#endif
