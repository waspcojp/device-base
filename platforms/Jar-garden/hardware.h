#ifndef	__INC_HARDWARE_H__
#define	__INC_HARDWARE_H__

#define DEVICE_CLASS            "jar-garden"

#define JAR_GARDEN
#define OLD_HARD

/*  sensors */
#define	HAVE_SENSORS
#define	HAVE_COOLING_FAN
#define HAVE_FAN
#define HAVE_LIGHT
#define HAVE_JAR_GARDEN
#define HAVE_BME280
#define HAVE_MLX90614

#define SEND_LIGHT_STATUS
#define USE_WIFI
#define USE_I2C

#define USE_OTA

#define	NR_RGBW_LED				1
#define	NR_NEOPIXEL_LED			0
#define	NR_LED					(NR_RGBW_LED + NR_NEOPIXEL_LED)
#define	NR_SENSORS				20
#define NR_COLOR                5

#define COLOR_R                 0
#define COLOR_G                 1
#define COLOR_B                 2
#define COLOR_W                 3
#define COLOR_DR                4

/*	port and channel	*/
#ifdef  OLD_HARD
#define	SCL_PORT				GPIO_NUM_16
#define	SDA_PORT				GPIO_NUM_17
#define	NEOPIXEL_LED_PORT		GPIO_NUM_18
#define	FAN_PORT				GPIO_NUM_19
#define	RGBW_LED_R_PORT			GPIO_NUM_22
#define	RGBW_LED_G_PORT			GPIO_NUM_21
#define	RGBW_LED_B_PORT			GPIO_NUM_5
#define	RGBW_LED_W_PORT			GPIO_NUM_4
#define	RGBW_LED_DR_PORT		GPIO_NUM_23
#else
#define	SCL_PORT				GPIO_NUM_16
#define	SDA_PORT				GPIO_NUM_17
#define	NEOPIXEL_LED_PORT		GPIO_NUM_33
#define	FAN_PORT				GPIO_NUM_4
#define	RGBW_LED_R_PORT			GPIO_NUM_27
#define	RGBW_LED_G_PORT			GPIO_NUM_32
#define	RGBW_LED_B_PORT			GPIO_NUM_12
#define	RGBW_LED_W_PORT			GPIO_NUM_25
#define	RGBW_LED_DR_PORT		GPIO_NUM_26
#endif
#define	RGBW_LED_TIMER_MODE		LEDC_HIGH_SPEED_MODE
#define	RGBW_LED_TIMER			LEDC_TIMER_0
#define	RGBW_LED_CH_R			LEDC_CHANNEL_0
#define	RGBW_LED_CH_G			LEDC_CHANNEL_1
#define	RGBW_LED_CH_B			LEDC_CHANNEL_2
#define	RGBW_LED_CH_W			LEDC_CHANNEL_3
#define	RGBW_LED_CH_DR			LEDC_CHANNEL_4

#define	FAN_TIMER_MODE			LEDC_HIGH_SPEED_MODE
#define	FAN_TIMER				LEDC_TIMER_1
#define	FAN_CHANNEL				LEDC_CHANNEL_5

// NeoPixel configure
#if ( NE_NOEPIXEL_LED > 0 )
#define	NEOPIXEL_SK6812
#define	NEOPIXEL_RMT_CHANNEL	RMT_CHANNEL_1
#endif

//	I2C configure
#define	I2C_FREQUENCY			100*1000
#define	I2C_PORT				I2C_NUM_0

#endif
