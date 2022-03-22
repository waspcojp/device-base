//#define	TRACE

#include 	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	"driver/ledc.h"
#include	"esp_err.h"
extern	"C"	{
#include 	"config.h"
#include	"types.h"
#include 	"debug.h"
#if	( NR_NEOPIXEL_LED > 0 )
#include	"neopixel.h"
#endif
#include	"globals.h"
#include	"misc.h"
}
#ifdef	HAVE_LIGHT
#include	"light.h"
#ifdef	SEND_SENSOR_STATUS
#include	"SensorInfo.h"
#endif


void
Light::initialize(void)
{
ENTER_FUNC;
#if	(NR_RGBW_LED > 0)
    ledc_timer_config_t ledc_timer = {
        .speed_mode = RGBW_LED_TIMER_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = RGBW_LED_TIMER,
        .freq_hz = 1000
    };
    ledc_channel_config_t ledc_channel[] = {
        {
            .gpio_num   = RGBW_LED_R_PORT,
            .speed_mode = RGBW_LED_TIMER_MODE,
            .channel    = RGBW_LED_CH_R,
            .timer_sel  = RGBW_LED_TIMER,
            .duty       = 0,
            .hpoint     = 0
        },{
            .gpio_num   = RGBW_LED_G_PORT,
            .speed_mode = RGBW_LED_TIMER_MODE,
            .channel    = RGBW_LED_CH_G,
            .timer_sel  = RGBW_LED_TIMER,
            .duty       = 0,
            .hpoint     = 0
        },{
            .gpio_num   = RGBW_LED_B_PORT,
            .speed_mode = RGBW_LED_TIMER_MODE,
            .channel    = RGBW_LED_CH_B,
            .timer_sel  = RGBW_LED_TIMER,
            .duty       = 0,
            .hpoint     = 0
        },{
            .gpio_num   = RGBW_LED_W_PORT,
            .speed_mode = RGBW_LED_TIMER_MODE,
            .channel    = RGBW_LED_CH_W,
            .timer_sel  = RGBW_LED_TIMER,
            .duty       = 0,
            .hpoint     = 0
#if	( NR_COLOR > 4 )
        },{
            .gpio_num   = RGBW_LED_DR_PORT,
            .speed_mode = RGBW_LED_TIMER_MODE,
            .channel    = RGBW_LED_CH_DR,
            .timer_sel  = RGBW_LED_TIMER,
            .duty       = 0,
            .hpoint     = 0
#endif
        },
	};

	ledc_timer_config(&ledc_timer);
	for (int ch = 0; ch < NR_COLOR; ch++) {
		ledc_channel_config(&ledc_channel[ch]);
		rgbw_RGBW[ch] = 0;
	}
	rgbw_need_update = FALSE;
#endif
#if	(NR_NEOPIXEL_LED > 0)
#if	0
	gpio_set_direction(NEOPIXEL_LED_PORT, GPIO_MODE_OUTPUT);
	gpio_set_level(NEOPIXEL_LED_PORT, 0);
	msleep(2);
#endif
	memset(&Px, 0, sizeof(Px));
	Px.pixels = (uint8_t *)malloc(sizeof(uint32_t) * NR_NEOPIXEL_LED);
	memclear(Px.pixels,sizeof(uint32_t) * NR_NEOPIXEL_LED);
	Px.pixel_count = NR_NEOPIXEL_LED;

	memset(&Px.timings, 0, sizeof(Px.timings));
	Px.timings.mark.level0 = 1;
	Px.timings.space.level0 = 1;
#ifdef	NEOPIXEL_PL9823
	strcpy(Px.color_order, "RGB");
	Px.nbits = 24;
	Px.timings.mark.duration0 = 27;
	Px.timings.mark.duration1 = 7;
	Px.timings.space.duration0 = 7;
	Px.timings.space.duration1 = 27;
	Px.timings.reset.duration0 = 1200;
	Px.timings.reset.duration1 = 1200;
#endif
#ifdef	NEOPIXEL_WS2812
	strcpy(Px.color_order, "GRB");
	Px.nbits = 24;
	Px.timings.mark.duration0 = 12;
	Px.timings.mark.duration1 = 14;
	Px.timings.space.duration0 = 7;
	Px.timings.space.duration1 = 16;
	Px.timings.reset.duration0 = 600;
	Px.timings.reset.duration1 = 600;
#endif
#ifdef	NEOPIXEL_SK6812
	strcpy(Px.color_order, "GRBW");
	Px.nbits = 32;
	Px.timings.mark.duration0 = 12;
	Px.timings.mark.duration1 = 12;
	Px.timings.space.duration0 = 6;
	Px.timings.space.duration1 = 18;
	Px.timings.reset.duration0 = 900;
	Px.timings.reset.duration1 = 900;
#endif
	Px.brightness = 0x80;
	neopixel_init(NEOPIXEL_LED_PORT, NEOPIXEL_RMT_CHANNEL);
	msleep(10);
	np_clear(&Px);
	np_show(&Px, NEOPIXEL_RMT_CHANNEL);
#endif
LEAVE_FUNC;
}

Light::Light()
{
	initialize();
}

void
Light::color(
	int		no,
	int		r,
	int		g,
	int		b,
	int		w,
	int		dr)
{
ENTER_FUNC;
#if	(NR_RGBW_LED > 0)
	ledc_set_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_R, r);
	ledc_set_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_G, g);
	ledc_set_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_B, b);
	ledc_set_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_W, w);
#if	( NR_COLOR > 4 )
	ledc_set_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_DR, dr);
#endif
	rgbw_RGBW[COLOR_R] = (uint8_t)r;
	rgbw_RGBW[COLOR_G] = (uint8_t)g;
	rgbw_RGBW[COLOR_B] = (uint8_t)b;
	rgbw_RGBW[COLOR_W] = (uint8_t)w;
	rgbw_RGBW[COLOR_DR] = (uint8_t)dr;
	rgbw_need_update = TRUE;
	no -= NR_RGBW_LED;
#endif
#if	(NR_NEOPIXEL_LED > 0 )
	np_set_pixel_rgbw(&Px, no, r, g, b, w);
#endif
LEAVE_FUNC;
}

void
Light::update()
{
ENTER_FUNC;
#if	(NR_RGBW_LED > 0)
	if	( rgbw_need_update )	{
		ledc_update_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_R);
		ledc_update_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_G);
		ledc_update_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_B);
		ledc_update_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_W);
#if	( NR_COLOR > 4 )
		ledc_update_duty(RGBW_LED_TIMER_MODE, RGBW_LED_CH_DR);
#endif
		rgbw_need_update = FALSE;
	}
#endif
#if	(NR_NEOPIXEL_LED > 0 )
	np_show(&Px, NEOPIXEL_RMT_CHANNEL);
#endif
LEAVE_FUNC;
}
			 
void
Light::color(
	int		r,
	int		g,
	int		b,
	int		w,
	int		dr)
{
	int		i;
ENTER_FUNC;
	for ( i = 0 ; i < NR_LED ; i ++ )	{
		color(i, r, g, b, w, dr);
	}
	update();
	msleep(200);
LEAVE_FUNC;
}

#ifdef	SEND_LIGHT_STATUS
void
Light::get(
	SenseBuffer *buff)
{
#if	(NR_RGBW_LED > 0)
	buff->put(&rgbw_RGBW, sizeof(uint8_t) * NR_COLOR);
#endif
#if	(NR_NEOPIXEL_LED > 0)
	uint8_t	rgbw[4];
	int		r, g, b, w;
	for	( int i = 0; i < NR_NEOPIXEL_LED; i ++ )	{
		np_get_pixel_rgbw(&Px, i, &r, &g, &b, &w);
		rgbw[0] = (uint8_t)r;
		rgbw[1] = (uint8_t)g;
		rgbw[2] = (uint8_t)b;
		rgbw[3] = (uint8_t)w;
		buff->put(&rgbw, sizeof(uint8_t) * 4);
	}
#endif
}

size_t
Light::build(
	char	*p,
	SenseBuffer	*buff)
{
	char	*q = p;
	uint8_t	color;;
ENTER_FUNC;
	p += sprintf(p, "\"value\":[");
	for ( int i = 0; i < NR_LED; i ++ )	{
		*p ++ = '[';
		for	( int j = 0; j < NR_COLOR ; j ++ )	{
			buff->get(&color, sizeof(uint8_t));
			p += sprintf(p, "%d", (int)color);
			if	( j  < NR_COLOR - 1 )	{
				*p ++ = ',';
			}
		}
		*p ++ = ']';
		if	( i < NR_LED - 1 )	{
			*p ++ = ',';
		}
	}
	*p ++ = ']';
LEAVE_FUNC;
	return	(p-q);
}

const	char	*
Light::name(void)
{
	return	"Light";
}

const	char	*
Light::data_class_name(void)
{
	return	"LightBrightness_RGBW";
}
#endif

#endif
