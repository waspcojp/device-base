#ifndef	__INC_UTILITY_H__
#define	__INC_UTILITY_H__

#include 	<limits.h>
#include 	<float.h>
#include	"types.h"

#define	CTOI(c)		((c)-'0')
#define	strlcmp(p,q)	strncmp((p),(q),strlen(q))

extern	float	atoF(char *str);
extern	char	*ftos(float val, int length, int prec);
extern	void	show_time(tTime *now, const char *msg);

#endif
