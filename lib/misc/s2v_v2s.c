/*
 *	s2v_v2s.c
 *		string -> value / value -> string routine for EPH
 *
 *	(C)opyright 1988,	all right reserved
 *		Dec.21, 1988	by S.Yamane
 *		Mar.01, 1989
 *      Dec.11, 1990    by K.Osada
 *
 *	$Header: S2V_V2S.Cv  1.7  90/12/30 00:48:10  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#if defined(_WIN32)
#	include	<string.h>
#else
#	include	<strings.h>
#endif

#include	"astrolib.h"
#include	"eph.h"

static char	*strmon[] = {
	NULL,
	"Jan. ",	"Feb. ",	"Mar. ",
	"Apr. ",	"May  ",	"June ",
	"July ",	"Aug. ",	"Sept.",
	"Oct. ",	"Nov. ",	"Dec. ",
};

int	strmon2mon( str )
char	*str;
{
	register int	i;

	for ( i=1; i <= 12; i++ ) {
		if ( strcmp( strmon[i], str ) == 0 )
			return ( i );
	}
	return ( 0 );
}

char	*mon2strmon( mon )
int	mon;
{
	return ( strmon[ mon ] );
}


/*
 *	day_of_week()
 *
 *	Dec.11, 1990   by K.Osada
 */
 
static char *week[] = {
	"Sun",	"Mon",	"Tue",	"Wed",	"Thu",	"Fri",	"Sat"
};

char *day_of_week( jd )
double	jd;
{
	int	week_number;

	week_number = (int)( fmod( jd+0.5+1.0, 7.0 ) );

	return ( week[ week_number ] );
}

/* *****************  end of s2v_v2s.c  **************** */
