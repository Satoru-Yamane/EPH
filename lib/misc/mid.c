/*
 *	mid.c :
 *
 *	(C)opyright 1988  by K.Osada
 *	   Nov 06, 1988
 *
 *	 Modified	   by S.Yamane ( Nov.11, 1988 )
 *
 *	rcsid: $Header: MID.Cv  1.4  90/11/26 13:36:16  yamane  Rel $
 */

#include  <stdio.h>
#include  <string.h>

char	*mid( s, start, n )
char	*s;
int	start,	n;
{
	register char	*p1,	*p2;
	int				len;
	static char		buf[ 300 ];

	len = strlen( s );
	if ( start > len )
		return ( NULL );
	if ( n > 300 )
		n = 300;

	for ( p1 = s+start, p2=buf; *p1 && n; n-- ) {
		*p2++ = *p1++;
	}
	*p2 = '\0';

	return ( buf );
}
/* ***** end of mid.c ***** */
