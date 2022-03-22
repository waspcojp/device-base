#include "config.default.h"

#include	"hardware.h"

/* SoftAP */
#define	DEFAULT_SSID			"JAR-GARDEN"
#define	DEFAULT_PASS			"jar-garden"
#define	DEFAULT_AP_SSID			""
#define	DEFAULT_AP_PASS			""

#if 0
#define	SERVER_ADDRESS	        "172.17.169.1"
#define	SERVER_NETMASK	        "255.255.255.0"
#define	SERVER_GATEWAY	        "172.17.169.1"
#define SERVER_PORT             8000
#else
#define	SERVER_ADDRESS	        "192.168.10.1"
#define	SERVER_NETMASK	        "255.255.255.0"
#define	SERVER_GATEWAY	        "192.168.10.1"
#define SERVER_PORT             8000
#endif

#define	MAX_STA_CONN	2

#define	CONSOLE_HOST			"https://service.device-console.com:8001"

#define ENDPOINT_SETTINGS       "/settings"
#define ENDPOINT_SCHEDULE       "/schedule"

#define	MAX_TIMER_EVENTS		100
#define MAX_RETRY               10
#define RETRY_INTERVAL          5

//#define	DEFAULT_SENSE_INTERVAL	3600
#define	DEFAULT_SENSE_INTERVAL	60

#define	N_TIMER_EVENTS			10

//#define	USE_LOCALCA

//#define TESTCODE
#define LINE_LENGTH             80

#ifdef TESTCODE
#define TESTCODE_CONVPPB
#endif

// disable command line and use voltage watch
// #define USE_V_WATCH