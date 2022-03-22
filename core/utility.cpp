#define	TRACE

#include 	<string.h>
#include	<stdio.h>
#include 	<stdlib.h>
#include 	<ctype.h>

#include 	"config.h"
#include 	"debug.h"
#include 	"utility.h"
#include 	"types.h"


#define	SIZE_OF_FLOAT		10

extern	float
atoF(
	char	*str)
{
	float	ret
		,	low
		,	mag;
	char	*p;
	Bool	minus;

	if	( *str == '-' )	{
		minus = TRUE;
		str ++;
	} else {
		minus = FALSE;
	}
	p = strchr(str, '.');
	if	( p != NULL )	{
		*p = 0;
	}
	ret = (float)atoi(str);
	if	( p != NULL )	{
		p ++;
		mag = 10000.0;
		low = 0.0;
		while	(	( *p != 0 )
				&&	( isdigit(CTOI(*p)) ) )	{
			mag *= 0.1;
			low += mag * CTOI(*p);
			p ++;
		}
		ret += low / 10000.0;
	}
	if	( minus )	{
		ret = - ret;
	}
	return	(ret);
}

extern	char	*
ftos(
	float	val,
	int		length,
	int		prec)
{
	static	char	buff[SIZE_OF_FLOAT+1];
	char	sign
		,	d;
	char	*p;
	float	pp;
	int		ip;
	bool	zero;
	

	if	( val < 0 )	{
		sign = 1;
		val = - val;
	} else {
		sign = 0;
	}
	if	(	( length == 0 )
		||	( length > SIZE_OF_FLOAT )
		||	( ( prec + 1 ) > length  ))	{
		p = buff;
		if		( sign > 0 )	{
			*p ++ = '-';
		}
		p += sprintf(p, "%d",(int)val);
		length = SIZE_OF_FLOAT;
	} else {
		p = buff + length - prec - 2;
		ip = (int)val;
		zero = false;
		while	( p >= buff )	{
			if		( zero )	{
				if		( sign == 1 )	{
					*p -- = '-';
					sign = 0;
				} else {
					*p -- = ' ';
				}
			} else {
				d = ip % 10;
				*p -- = d + '0';
				ip = ip / 10;
			}
			if	( ip == 0 )	{
				zero = true;
			}
		}
		p = buff + length - prec - 1;
	}
	if		( prec != 0 )	{
		*p ++ = '.';
	}
	pp = (int)val;
	pp = val - (float)pp;
	while	( ( p - buff ) < length )	{
		if	( prec == 0 )	break;
		pp *= 10;
		d = (int)pp;
		*p ++ = d + '0';
		pp -= d;
		prec --;
	}
	*p = 0;
	return	buff;
}

extern	void
show_time(
	tTime	*now,
	const char	*msg)
{
	if	( msg == NULL )	{
		dbgprintf("%d/%d/%d - %02d:%02d:%02d",
				  now->tm_year,
				  now->tm_mon + 1,
				  now->tm_mday,
				  now->tm_hour,
				  now->tm_min,
				  now->tm_sec);
	} else {
		dbgprintf("%d/%d/%d - %02d:%02d:%02d:%s",
				  now->tm_year,
				  now->tm_mon + 1,
				  now->tm_mday,
				  now->tm_hour,
				  now->tm_min,
				  now->tm_sec,
				  msg);
	}
}

