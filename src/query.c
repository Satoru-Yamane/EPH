/*
 *	query.c
 *		query routine for EPH
 *
 *	(C)opyright 1991	all right reserved
 *      Jan.05, 1991		by S.Yamane
 *
 *	$Header: QUERY.Cv  1.4  91/02/08 10:36:04  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"filer.h"
#include	"getval.h"
#include	"ephcurs.h"


#define	KEYSTR_X0		4
#define	KEYSTR_X1		KEYSTR_X0+39
#define	KEYSTR_Y0		4

static Orbit_element	w_el;
static Comment			w_cm;

#ifdef USE_REGEX
static Query	Queries[] = {
	{	 w_el.roman,		VAL_STRING,	NULL,	},
	{	 w_el.number,		VAL_STRING,	NULL,	},
	{	 w_el.name,		VAL_STRING,	NULL,	},
	{	&w_el.epoch,		VAL_SHORT_JD,	NULL,	},
	{	&w_el.mean_anomaly,	VAL_DEGREE,	NULL,	},
	{	&w_el.time_pass,	VAL_SHORT_JD,	NULL,	},
	{	&w_el.peri,		VAL_DEGREE,	NULL,	},
	{	&w_el.node,		VAL_DEGREE,	NULL,	},
	{	&w_el.incl,		VAL_DEGREE,	NULL,	},
	{	&w_el.peri_dis,		VAL_DOUBLE,	NULL,	},
	{	&w_el.ecc,		VAL_DOUBLE,	NULL,	},
	{	&w_el.sm_axis,		VAL_DOUBLE,	NULL,	},
	{	&w_el.daily_motion,	VAL_DEGREE,	NULL,	},
	{	&w_el.period,		VAL_DOUBLE,	NULL,	},
	{	 w_cm.note,		VAL_STRING,	NULL,	},
};
#else
static Query	Queries[] = {
	{	 w_el.roman,		VAL_STRING,	},
	{	 w_el.number,		VAL_STRING,	},
	{	 w_el.name,		VAL_STRING,	},
	{	&w_el.epoch,		VAL_SHORT_JD,	},
	{	&w_el.mean_anomaly,	VAL_DEGREE,	},
	{	&w_el.time_pass,	VAL_SHORT_JD,	},
	{	&w_el.peri,		VAL_DEGREE,	},
	{	&w_el.node,		VAL_DEGREE,	},
	{	&w_el.incl,		VAL_DEGREE,	},
	{	&w_el.peri_dis,		VAL_DOUBLE,	},
	{	&w_el.ecc,		VAL_DOUBLE,	},
	{	&w_el.sm_axis,		VAL_DOUBLE,	},
	{	&w_el.daily_motion,	VAL_DEGREE,	},
	{	&w_el.period,		VAL_DOUBLE,	},
	{	 w_cm.note,		VAL_STRING,	},
};
#endif

#ifdef __BORLANDC__
static Field	key_var[] = {
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+0,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+1,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+2,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+3,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+4,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+5,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+6,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+7,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+8,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+9,  KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+10, KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+11, KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+12, KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+13, KEYSTR_MAX, "%s", NULL, },
	{NULL, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+14, KEYSTR_MAX, "%s", NULL, },
	{NULL, 0,	   -1,	      -1,	    -1,		NULL, NULL, },
} ;
#else
static Field	key_var[] = {
	{Queries[0].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+0,  KEYSTR_MAX, "%s", NULL, },
	{Queries[1].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+1,  KEYSTR_MAX, "%s", NULL, },
	{Queries[2].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+2,  KEYSTR_MAX, "%s", NULL, },
	{Queries[3].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+3,  KEYSTR_MAX, "%s", NULL, },
	{Queries[4].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+4,  KEYSTR_MAX, "%s", NULL, },
	{Queries[5].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+5,  KEYSTR_MAX, "%s", NULL, },
	{Queries[6].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+6,  KEYSTR_MAX, "%s", NULL, },
	{Queries[7].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+7,  KEYSTR_MAX, "%s", NULL, },
	{Queries[8].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+8,  KEYSTR_MAX, "%s", NULL, },
	{Queries[9].str, VAL_STRING, KEYSTR_X1, KEYSTR_Y0+9,  KEYSTR_MAX, "%s", NULL, },
	{Queries[10].str,VAL_STRING, KEYSTR_X1, KEYSTR_Y0+10, KEYSTR_MAX, "%s", NULL, },
	{Queries[11].str,VAL_STRING, KEYSTR_X1, KEYSTR_Y0+11, KEYSTR_MAX, "%s", NULL, },
	{Queries[12].str,VAL_STRING, KEYSTR_X1, KEYSTR_Y0+12, KEYSTR_MAX, "%s", NULL, },
	{Queries[13].str,VAL_STRING, KEYSTR_X1, KEYSTR_Y0+13, KEYSTR_MAX, "%s", NULL, },
	{Queries[14].str,VAL_STRING, KEYSTR_X1, KEYSTR_Y0+14, KEYSTR_MAX, "%s", NULL, },
	{NULL,		 0,	     -1,	-1,	      -1,	  NULL, NULL, },
} ;
#endif

static MenuFunc	scan_go()
{
	return ( QUIT | NO_REWRITE );
}

static Menu	keystr_menu[] = {
	{ AUTOEXEC,	'\0',	"Roman numeral designation       [S]",	getval,	&key_var[ 0] },
	{ AUTOEXEC,	'\0',	"Provisional designation / Number[S]",	getval,	&key_var[ 1] },
	{ AUTOEXEC,	'\0',	"Comet / Minor planet Name       [S]",	getval,	&key_var[ 2] },
	{ AUTOEXEC,	'\0',	"Osculation epoch         (Epoch)[N]",	getval,	&key_var[ 3] },
	{ AUTOEXEC,	'\0',	"Mean anomaly             (  M  )[N]",	getval,	&key_var[ 4] },
	{ AUTOEXEC,	'\0',	"Time of peri. passage    (  T  )[N]",	getval,	&key_var[ 5] },
	{ AUTOEXEC,	'\0',	"Argument of perihelion   (peri.)[N]",	getval,	&key_var[ 6] },
	{ AUTOEXEC,	'\0',	"Longitude of asc. node   (node )[N]",	getval,	&key_var[ 7] },
	{ AUTOEXEC,	'\0',	"Inclination              (incl.)[N]",	getval,	&key_var[ 8] },
	{ AUTOEXEC,	'\0',	"Perihelion distance      (  q  )[N]",	getval,	&key_var[ 9] },
	{ AUTOEXEC,	'\0',	"Eccentricity             (  e  )[N]",	getval,	&key_var[10] },
	{ AUTOEXEC,	'\0',	"Semi-major axis          (  a  )[N]",	getval,	&key_var[11] },
	{ AUTOEXEC,	'\0',	"Mean Daily motion        (  n  )[N]",	getval,	&key_var[12] },
	{ AUTOEXEC,	'\0',	"Period                   (  P  )[N]",	getval,	&key_var[13] },
	{ AUTOEXEC,	'\0',	"Note                            [S]",	getval,	&key_var[14] },
	{ WAIT_KEY,	'\0',	"Scan Go                            ",	scan_go,	NULL	 },
	{ WAIT_KEY,	'\0',	"To Upper Menu                      ",	menu_upper,	NULL	 },
	{ NOT_EXEC,	'\0',	NULL,									NULL,		NULL	 },
} ;

MenuFunc	get_keystr( recsize )
int	recsize;
{
	register int	i;
	int				x=KEYSTR_X0,	y=KEYSTR_Y0;
	int				result,	deflt,	No_comment,	dim;

	clear_bottom( 0, y-2 );
	frame_box(x-2, y-1, 72, 19);
	if ( ( ctrl_sw.ent_obj == COMET_IN && recsize == COMET_LONG )
	  || ( ctrl_sw.ent_obj == ASTEROID_IN
	    && ( recsize == MPLA_IAU_EXT || recsize == MPLA_LONG ) ) )
		No_comment = FALSE;
	else
		No_comment = TRUE;

#ifdef __BORLANDC__
	for ( i = 0; i < 15; i++ ) {
		key_var[i].varptr = Queries[i].str;
		}
#endif
	for ( i = 0; key_var[i].varptr != NULL; i++ ) {
		keystr_menu[i].exe_flg = AUTOEXEC;
		if ( global_sw.exec_mode != EXEC_LIB
		  && Queries[i].vartype != VAL_STRING )
			keystr_menu[i].exe_flg = NOT_EXEC;

		if ( i == 14 )		/* note */
			keystr_menu[i].exe_flg = ( No_comment ) ? NOT_EXEC : AUTOEXEC;

		dim = ( keystr_menu[i].exe_flg == NOT_EXEC ) ? 1 : 0;
		key_var[i].changed = 0;
		init_query( Queries+i );
		putval( &key_var[i], dim );
	}

	result = 0;
	deflt = 15;
	guidance( GUID_QUERY );
	do {
		result = menu_man( keystr_menu, x, y, MN_INPUT, deflt );
		deflt  = ITEM_NO( result );
	} while ( ! ( result & UPPER_MENU || result & QUIT ) ) ;

	if ( ( result & QUIT ) && ( result & NO_REWRITE ) )
		result = 0;

	for ( i = 0; i < sizeof(Queries)/sizeof(Queries[0]); i++ ) {
		parse_query( Queries+i, keystr_menu[i].exe_flg == NOT_EXEC ? FALSE : TRUE );
	}
	clear_bottom( x-2, y-2 );

	return ( result );
}


int	MatchKey( el, cm )
Orbit_element	*el;
Comment			*cm;
{
	int				i,	result;
	unsigned short	HasMin,	HasMax;
	int				type,	w;

	w_el = *el;
	w_cm = *cm;
/*
	memcpy( &w_el, (char *)el, sizeof(Orbit_element) );
	memcpy( &w_cm, (char *)cm, sizeof(Comment) );
 */

	result = TRUE;
	for ( i = 0; result == TRUE && i < sizeof(Queries)/sizeof(Queries[0]); i++ ) {
		if ( keystr_menu[i].exe_flg != AUTOEXEC )
			continue;

		result = match_query( &Queries[i] );
	}

	return ( result );
}

/* *******************  End of query.c  ****************** */
