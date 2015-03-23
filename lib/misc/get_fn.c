/*
 *	getfn.c :
 *		Get filename routine for EPH
 *
 *	(C)opyright 1989,  all right reserved
 *		Jan.13, 1989	  by S.Yamane
 *
 *	$Header: GET_FN.Cv  1.10  91/01/11 17:28:32  yamane  Rel $
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"ephcurs.h"

/*
 *	functions
 */
int
clip_blanks( str )
char	*str;
{
	register int	n = 0;
	register char	*p;

	p = str + strlen( str );
	do {
		*(p--) = '\0';
		n++;
	} while ( *p && *p == ' ' );

	return ( n );
}

/*
 *	get_fname ---
 *		Get filename for menu
 */
MenuFunc
get_fname( param )
Fname	*param;
{
	int		ik,	chk,	retval;
	FILE	*dmy;
	CursPos	curs_pos;

	push_cursor(&curs_pos);

	chk = ! TRUE;
	do {
		ik = fmt_gets( param->xpos, param->ypos+Y_BIAS,
						NULL, param->varptr, 40 );
					/*	FN_FORMAT, param->varptr, 40 ); */

		if ( ik == ESCAPE || ik == ctrl('C') ) {
			chk = TRUE;
			retval = menu_upper();
		} else {
			retval = ( ik == UP_ARROW ? FORE_ITEM : NEXT_ITEM );
			clip_blanks( param->varptr );
			dmy = fopen( param->varptr, "r" );
			if ( dmy )
				fclose( dmy );

			if ( param->flg & 0x00ff ) {		/* write mode check */
				if ( dmy == NULL ) {
					chk = TRUE;
				} else {
					ik = warning( "Overwrite ?(y/n)" );
					if ( isupper(ik) )
						ik = tolower(ik);

					if ( ik == 'y' )
						chk = TRUE;
				}
			} else {
				if ( dmy == NULL ) {
					ik = warning( "No such file:Ignore ?(y/n)" );
					if ( isupper(ik) )
						ik = tolower(ik);

					if ( ik == 'y' )
						chk = TRUE;			/* ignore if Not exist */
				} else {
					chk = TRUE;
				}
			}
		}
	} while ( chk != TRUE );

	pop_cursor(&curs_pos);

	return( retval | NO_REWRITE );
}

/* ****************  end of getfn.c  ************* */
