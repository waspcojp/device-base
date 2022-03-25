#ifndef	__INC_HARDWARE_H__
#define	__INC_HARDWARE_H__

#define DEVICE_CLASS            "template"

/*  sensors */
#define	HAVE_SENSORS
#define USE_WIFI
//#define USE_I2C

#define USE_OTA

#define	NR_SENSORS				20

/*	port and channel	*/
#ifdef  USE_I2C
//#define	SCL_PORT				GPIO_NUM_16
//#define	SDA_PORT				GPIO_NUM_17
#endif

// NeoPixel configure
#if ( NE_NOEPIXEL_LED > 0 )
#define	NEOPIXEL_SK6812
#define	NEOPIXEL_RMT_CHANNEL	RMT_CHANNEL_1
#endif

//	I2C configure
#define	I2C_FREQUENCY			100*1000
#define	I2C_PORT				I2C_NUM_0

#endif
