/*
 *	input.c
 *		input routine for EPH
 *
 *	(C)opyright 1988-1990  all right reserved
 *		Dec.21, 1988	  by S.Yamane
 *      Apr.16, 1989      by K.Osada
 *      Dec.03, 1989      by K.Osada
 *
 *	$Header: INPUT.Cv  1.16  91/02/08 10:18:40  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<string.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"getval.h"
#include	"ephcurs.h"

/*
 *	ent_date()
 *		Current Date for Ephemeris entry/edit routine
 *
 *	   Jan. 12, 1989		by S.Yamane
 */
#define	ENT_DATE_BASELINE		13

static int	after_eph_start();

static Field	date_var[] = {
	{ &current.ut_et,		VAL_DOUBLE,		39, ENT_DATE_BASELINE+1,	18,
	   "%18.6lf",
	   "        +##.######",
	   NULL,
	},
	{ &current.start_jd,	VAL_LONG_JD,	39, ENT_DATE_BASELINE+2,	20,
	   "% 05d/%02d/%02d %02d:%02d:%02d",
	   "+####/##/## ##:##:##",
	   after_eph_start,
	},
	{ &current.end_jd,		VAL_LONG_JD,	39, ENT_DATE_BASELINE+3,	20,
	   "% 05d/%02d/%02d %02d:%02d:%02d",
	   "+####/##/## ##:##:##",
	   NULL,
	},
	{ &current.step,		VAL_DOUBLE,		39, ENT_DATE_BASELINE+4,	17,
	   "%17.5lf",
	   "       +###.#####",
	   NULL,
	},
	{  NULL,				0,				-1, -1,	-1,
	   NULL,
	   NULL,
	   NULL,
	},
/* ***************************************
 *	if step by time mode,
 *		swap date_var[3] with this one.
 * *************************************** */
	{ &current.step,		VAL_TIME,		39, ENT_DATE_BASELINE+4,	20,
	   "           % 03d:%02d:%02d",
	   "           +##:##:##",
	   NULL,
	},
} ;

static Menu	date_menu[] = {
	{ NOT_EXEC,	'\0',	"TT-UT[day]",		getval,		&date_var[0]	},
	{ AUTOEXEC,	'\0',	"Date start",		getval,		&date_var[1]	},
	{ AUTOEXEC,	'\0',	"Date end  ",		getval,		&date_var[2]	},
	{ AUTOEXEC,	'\0',	"Step[days]",		getval,		&date_var[3]	},
	{ WAIT_KEY,	'\0',	"Upper-Menu",		menu_upper,	NULL		},
	{ NOT_EXEC,	'\0',	NULL,				NULL,		NULL		},
/* ***************************************
 *	if step by time mode,
 *		swap date_manu[3] with this one.
 * *************************************** */
	{ AUTOEXEC,	'\0',	"Step[time]",		getval,		&date_var[3]	},
} ;

static int	after_eph_start( dmy )
Field	*dmy;
{
	if ( ctrl_sw.ut2et_sw == 0 ) {		/* automatic UT->TT conversion */
/*
		current.ut_et = ut2et( current.start_jd );
*/
		current.ut_et = et2ut( current.start_jd );
		putval( &date_var[0], 1 );		/* dim */
	}
	if ( ctrl_sw.eph_mode == 1 ) {		/* Single date mode */
		current.end_jd = current.start_jd;
		putval( &date_var[2], 1 );		/* dim */
	}

	return ( 0 );
}

int	ent_date()
{
	register int	i;
	int			x = 25,	y = ENT_DATE_BASELINE + 1;
	int			result,	deflt;
	int			ik;
	double		w_ut_et;
	Field	wv;			/* for swapping */
	Menu		wm;			/*	 "		*/

	top_bar( TB_ED_DATE );
	guidance( GUID_KEYIN );
	clear_bottom( 0, y-2 );
	frame_box(x-2, y-1, 38, 7);

	if ( ctrl_sw.eph_step_sw == 1 ) {	/* step by time mode */
		if ( date_var[3].vartype == VAL_DOUBLE ) {
			wv=date_var[3];	date_var[3] =date_var[5];	date_var[5] =wv;
			wm=date_menu[3];   date_menu[3]=date_menu[6];   date_menu[6]=wm;
		}
	}
	if ( ctrl_sw.eph_step_sw == 0 ) {	/* step by day mode */
		if ( date_var[3].vartype == VAL_TIME ) {
			wv=date_var[3];	date_var[3] =date_var[5];	date_var[5] =wv;
			wm=date_menu[3];   date_menu[3]=date_menu[6];   date_menu[6]=wm;
		}
	}
	date_menu[0].exe_flg = ( ctrl_sw.ut2et_sw == 0 ? NOT_EXEC : AUTOEXEC );
	date_menu[2].exe_flg = ( ctrl_sw.eph_mode == 1 ? NOT_EXEC : AUTOEXEC );
	date_menu[3].exe_flg = ( ctrl_sw.eph_mode == 1 ? NOT_EXEC : AUTOEXEC );

	for ( i = 0; date_var[i].varptr != NULL; i++ ) {
		int	dim;

		dim = ( date_menu[i].exe_flg == NOT_EXEC ) ? 1 : 0;
		putval( &date_var[i], dim );
		date_var[i].changed = 0;
	}
	w_ut_et = current.ut_et;
	result = deflt = 0;
	do {
		result = menu_man( date_menu, x, y, MN_INPUT, deflt );
		deflt  = ITEM_NO( result );
		if ( ( current.start_jd > current.end_jd && current.step >= 0.0 )
		 || ( current.start_jd < current.end_jd && current.step <= 0.0 ) ){
			result = warning( "entry error !!" );
		}
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	guidance( GUID_SUB );

	if ( current.ut_et != w_ut_et		/* UT -> ET */
	  || date_var[1].changed			/* Start date */
	  || date_var[3].changed ) {		/* Step	   */
		arry_max = 0;				/* recalculate */
	}

	return ( 0 );
}


/*
 *	ent_id_date()
 *		Current Date for Identification entry/edit routine
 *
 *	   Jan. 12, 1989		by S.Yamane
 */
#define	ENT_ID_DATE_BASE		15
static int	after_id_start();

static Field	id_date_var[] = {
	{ &current.id_ut_et,			VAL_DOUBLE,		39, ENT_ID_DATE_BASE,	18,
	   "%18.6lf",
	   "        +##.######",
	   NULL,
	},
	{ &current.id_start_jd,	VAL_LONG_JD,	39, ENT_ID_DATE_BASE+1,	20,
	   "% 05d/%02d/%02d %02d:%02d:%02d",
	   "+####/##/## ##:##:##",
	   after_id_start,
	},
	{  NULL,					0,				-1, -1,	-1,
	   NULL,
	   NULL,
	   NULL,
	},
} ;

static Menu	id_date_menu[] = {
	{ NOT_EXEC,	'\0',	"TT-UT[day]",		getval,		&id_date_var[0]	},
	{ AUTOEXEC,	'\0',	"Date      ",		getval,		&id_date_var[1]	},
	{ WAIT_KEY,	'\0',	"Upper-Menu",		menu_upper,	NULL		},
	{ NOT_EXEC,	'\0',	NULL,				NULL,		NULL		},
} ;

static int	after_id_start( dmy )
Field	*dmy;
{
	if ( ctrl_sw.ut2et_sw == 0 ) {		/* automatic UT->TT conversion */
		current.id_ut_et = ut2et( current.id_start_jd );
		putval( &id_date_var[0], 1 );		/* dim */
	}

	return ( 0 );
}

int	ent_id_date()
{
	register int	i;
	int				x=25,	y=ENT_ID_DATE_BASE;
	int				result,	deflt;
	int				ik,		dim;

	top_bar( TB_ED_DATE );
	guidance( GUID_KEYIN );
	clear_bottom( x-2, y-1 );

	frame_box(x-2, y-1, 38, 5);
	id_date_menu[0].exe_flg = ( ctrl_sw.ut2et_sw == 0 ? NOT_EXEC : AUTOEXEC );
	for ( i = 0; id_date_var[i].varptr != NULL; i++ ) {
		dim = ( id_date_menu[i].exe_flg == NOT_EXEC ) ? 1 : 0;
		putval( &id_date_var[i], dim );
	}

	result = deflt = 0;
	do {
		result = menu_man( id_date_menu, x, y, MN_INPUT, deflt );
		deflt  = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	guidance( GUID_SUB );

	return ( 0 );
}



/*
 *	ent_orbit()
 *		Orbital element entry/edit routine
 *
 *	   Dec. 28, 1988		by S.Yamane
 *	Modified		by K.Osada ( Apr.16, 1989 )
 */

#define		TOP_LINE		2
static int	after_epoch();
static int	after_M();
static int	after_q();
static int	after_ecc();
static int	after_axis();
static int	after_h1();

static Field	el_var[] = {
	{  orb_el_buf.roman,		VAL_STRING,	46,	TOP_LINE,	11,
	   "%s",
	   "???????????",
	   NULL,
	},
	{  orb_el_buf.number,		VAL_STRING,	46,	TOP_LINE+1,	10,
	   "%s",
	   "??????????",
	   NULL,
	},
	{  orb_el_buf.name,		VAL_STRING,	46,	TOP_LINE+2,	30,
	   "%s",
	   NULL,
	   NULL,
	},
	{ &orb_el_buf.epoch,		VAL_SHORT_JD,	45,	TOP_LINE+3,	13,
	   "% 05d/%02d/%04.1lf",
	   "+####/##/##.#",
	   after_epoch,
	},
	{ &orb_el_buf.mean_anomaly,	VAL_DEGREE,	48,	TOP_LINE+4,	14,
	   "%14.5lf",
	   "     ###.#####",
	   after_M,
	},
	{ &orb_el_buf.time_pass,	VAL_SHORT_JD,	45,	TOP_LINE+5,	17,
	   "% 05d/%02d/%08.5lf",
	   "+####/##/##.#####",
	   NULL,
	},
	{ &orb_el_buf.peri,		VAL_DEGREE,	48,	TOP_LINE+6,	14,
	   "     %09.5lf",
	   "     ###.#####",
	   NULL,
	},
	{ &orb_el_buf.node,		VAL_DEGREE,	48,	TOP_LINE+7,	14,
	   "     %09.5lf",
	   "     ###.#####",
	   NULL,
	},
	{ &orb_el_buf.incl,		VAL_DEGREE,	48,	TOP_LINE+8,	14,
	   "%14.5lf",
	   "     ###.#####",
	   NULL,
	},
	{ &orb_el_buf.peri_dis,		VAL_DOUBLE,	48,	TOP_LINE+9,	16,
	   "%16.7lf",
	   "      +#.#######",
	   after_q,
	},
	{ &orb_el_buf.ecc,		VAL_DOUBLE,	48,	TOP_LINE+10,	16,
	   "%16.7lf",
	   "      +#.#######",
	   after_ecc,
	},
	{ &orb_el_buf.sm_axis,		VAL_DOUBLE,	48,	TOP_LINE+11,	16,
	   "%16.7lf",
	   "    ####.#######",
	   after_axis,
	},
	{ &orb_el_buf.daily_motion,	VAL_DEGREE,	48,	TOP_LINE+12,	17,
	   "%17.8lf",
	   "     ###.########",
	   NULL,
	},
	{ &orb_el_buf.period,		VAL_DOUBLE,	48,	TOP_LINE+13,	11,
	   "%11.2lf",
	   "     ###.##",
	   NULL,
	},
	{ &orb_el_buf.h1,		VAL_DOUBLE,	48,	TOP_LINE+14,	11,
	   "%11.2lf",
	   "      ##.##",
	   after_h1,
	},
	{ &orb_el_buf.k1,		VAL_DOUBLE,	48,	TOP_LINE+15,	11,
	   "%11.2lf",
	   "      ##.##",
	   NULL,
	},
	{ &orb_el_buf.h2,		VAL_DOUBLE,	48,	TOP_LINE+16,	11,
	   "%11.2lf",
	   "      ##.##",
	   NULL,
	},
	{ &orb_el_buf.k2,		VAL_DOUBLE,	48,	TOP_LINE+17,	11,
	   "%11.2lf",
	   "      ##.##",
	   NULL,
	},
	{ &orb_el_buf.g,		VAL_DOUBLE,	48,	TOP_LINE+18,	11,
	   "%11.2lf",
	   "       #.##",
	   NULL,
	},
	{ &orb_el_buf.tail,		VAL_DOUBLE,	48,	TOP_LINE+19,	14,
	   "%14.5lf",	
	   "      ##.#####",
	   NULL,
	},
	{  NULL,			0,		-1,	-1,		-1,
	   NULL,
	   NULL,
	   NULL,
	},
} ;

static Menu	el_menu[] = {
	{ AUTOEXEC,	'\0',	"Roman numeral designation            ",getval,	&el_var[ 0] },
	{ AUTOEXEC,	'\0',	"Provisional designation / Number     ",getval, &el_var[ 1] },
	{ AUTOEXEC,	'\0',	"Comet / Minor planet name",			getval,	&el_var[ 2] },
	{ AUTOEXEC,	'\0',	"Osculation epoch    [TT or ET](Epoch)",getval, &el_var[ 3] },
	{ AUTOEXEC,	'\0',	"Mean anomaly           [deg]   (  M )",getval,	&el_var[ 4] },
	{ AUTOEXEC,	'\0',	"Time of peri. passage[TT or ET](  T )",getval,	&el_var[ 5] },
	{ AUTOEXEC,	'\0',	"Argument of perihelion [deg]  (peri.)",getval,	&el_var[ 6] },
	{ AUTOEXEC,	'\0',	"Longitude of asc. node [deg]  (node )",getval,	&el_var[ 7] },
	{ AUTOEXEC,	'\0',	"Inclination            [deg]  (incl.)",getval,	&el_var[ 8] },
	{ AUTOEXEC,	'\0',	"Perihelion distance    [AU]    (  q )",getval,	&el_var[ 9] },
	{ AUTOEXEC,	'\0',	"Eccentricity                   (  e )",getval,	&el_var[10] },
	{ AUTOEXEC,	'\0',	"Semi-major axis        [AU]    (  a )",getval,	&el_var[11] },
	{ AUTOEXEC,	'\0',	"Mean Daily motion              (  n )",getval,	&el_var[12] },
	{ AUTOEXEC,	'\0',	"Period                 [Years] (  P )",getval,	&el_var[13] },
	{ AUTOEXEC,	'\0',	"Magnitude[m1] parameter        ( H1 )",getval,	&el_var[14] },
	{ AUTOEXEC,	'\0',	"            \"                  ( K1 )",getval,&el_var[15] },
	{ AUTOEXEC,	'\0',	"Magnitude[m2/V/B] parameter  (H2/H/B)",getval,	&el_var[16] },
	{ AUTOEXEC,	'\0',	"            \"                  ( K2 )",getval,&el_var[17] },
	{ AUTOEXEC,	'\0',	"Slope parameter                ( G  )",getval,	&el_var[18] },
	{ AUTOEXEC,	'\0',	"Tail length            [AU]          ",getval,	&el_var[19] },
	{ WAIT_KEY, '\0',	"Upper-Menu                           ",menu_upper,  NULL   },
	{ NOT_EXEC,	'\0',	NULL,									NULL,	NULL		},
} ;

static int	flgval[][3] = {
/*	- comet -  - Mi.Pla. - - Ecc.Mi.Pla. -	*/
	{  AUTOEXEC,   NOT_EXEC,   NOT_EXEC  },			/* Roman		*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* Number		*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* Name			*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* Epoch		*/
	{  NOT_EXEC,   AUTOEXEC,   NOT_EXEC  },			/* Mean anomary	*/
	{  AUTOEXEC,   NOT_EXEC,   AUTOEXEC  },			/* T.			*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* peri.		*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* node.		*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* incl.		*/
	{  AUTOEXEC,   NOT_EXEC,   AUTOEXEC  },			/* Peri. Dist.	*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* e			*/
	{  NOT_EXEC,   AUTOEXEC,   NOT_EXEC  },			/* a			*/
	{  NOT_EXEC,   NOT_EXEC,   NOT_EXEC  },			/* Daily motion	*/
	{  NOT_EXEC,   NOT_EXEC,   NOT_EXEC  },			/* P			*/
	{  AUTOEXEC,   NOT_EXEC,   NOT_EXEC  },			/* h1			*/
	{  AUTOEXEC,   NOT_EXEC,   NOT_EXEC  },			/* k1			*/
	{  AUTOEXEC,   AUTOEXEC,   AUTOEXEC  },			/* h2			*/
	{  AUTOEXEC,   NOT_EXEC,   NOT_EXEC  },			/* k2			*/
	{  NOT_EXEC,   AUTOEXEC,   AUTOEXEC  },			/* G			*/
	{  AUTOEXEC,   NOT_EXEC,   NOT_EXEC  },			/* Tail length	*/
	{  WAIT_KEY,   WAIT_KEY,   WAIT_KEY  },			/* Upper-menu	*/
	{  NOT_EXEC,   NOT_EXEC,   NOT_EXEC  },			/* terminater	*/
} ;

static int	after_epoch( dmy )
Field	*dmy;
{
	int		dim = 0;

	if ( ctrl_sw.ent_obj == ASTEROID_IN ) {
		if ( orb_el_buf.daily_motion != 0.0 ) {
			orb_el_buf.time_pass = orb_el_buf.epoch
					  - orb_el_buf.mean_anomaly / orb_el_buf.daily_motion;
		} else {
			orb_el_buf.time_pass = orb_el_buf.epoch;
		}
		dim = ( ctrl_sw.ent_obj == ASTEROID_IN ) ? 1 : 0;
		putval( &el_var[ 5], dim );	/* time_pass	*/
	}

	return ( 0 );
}

static int	after_M( dmy )
Field	*dmy;
{
	int		dim = 0;

	if ( orb_el_buf.daily_motion != 0.0 ) {
		orb_el_buf.time_pass = orb_el_buf.epoch
					   - orb_el_buf.mean_anomaly / orb_el_buf.daily_motion;
	} else {
		orb_el_buf.time_pass = orb_el_buf.epoch;
	}
	dim = ( ctrl_sw.ent_obj == ASTEROID_IN ) ? 1 : 0;
	putval( &el_var[ 5], dim );		/* time_pass */

	return ( 0 );
}

static int	after_q( dmy )
Field	*dmy;
{
	int		dim = 0;
	double  w;

	if ( orb_el_buf.ecc < 1.0 ) {
		orb_el_buf.sm_axis =  orb_el_buf.peri_dis / ( 1.0 - orb_el_buf.ecc );
		if ( orb_el_buf.sm_axis >= 0.0 ) {
			w = sqrt( orb_el_buf.sm_axis ) * orb_el_buf.sm_axis;
			orb_el_buf.period  = w;
			if ( w > 0.0 ) {
				orb_el_buf.daily_motion =  DEG2RAD( 0.985607669 / w );
				dim = ( ctrl_sw.ent_obj == ASTEROID_IN ) ? 0 : 1;
				putval( &el_var[11], dim );	/* semi-major axis	*/
				putval( &el_var[12], dim );	/* daily motion		*/
				dim = 1;
				putval( &el_var[13], dim );	/* period		*/
			}
		}
	}

	return ( 0 );
}

static int	after_ecc( dmy )
Field	*dmy;
{
	int		dim = 0;
	double	w;

	w = 1.0 - orb_el_buf.ecc;
	if ( ctrl_sw.ent_obj == ASTEROID_IN ) {
		dim = 1;
		orb_el_buf.peri_dis = orb_el_buf.sm_axis * w;
		el_var[ 9].hyphen = ( orb_el_buf.peri_dis >= 0.0 ) ? 0 : HYPHEN( 5,6,6 );
		putval( &el_var[ 9], dim );			/* peri_dis */
		if ( orb_el_buf.ecc >=1.0 ) {
			 el_var[11].hyphen = HYPHEN( 5, 6, 6 ); /* semi-major axis	  */
			 el_var[12].hyphen = HYPHEN( 5, 6, 6 ); /* daily motion		 */
			 el_var[13].hyphen = HYPHEN( 5, 6, 6 ); /* period			   */
		 } else {
			 el_var[11].hyphen = 0;				 /* semi-major axis	  */
			 el_var[12].hyphen = 0;				 /* daily motion		 */
			 el_var[13].hyphen = 0;				 /* period			   */
		 }
	} else {
		el_var[11].hyphen = HYPHEN( 5, 6, 6 );	/* semi-major axis	*/
		el_var[12].hyphen = HYPHEN( 5, 6, 6 );	/* daily motion		*/
		el_var[13].hyphen = HYPHEN( 5, 6, 6 );	/* period		*/

		if ( w > 0.0 ) {
			orb_el_buf.sm_axis =  orb_el_buf.peri_dis / w;
			el_var[11].hyphen = 0;		/* semi-major axis	*/
			if ( orb_el_buf.sm_axis >= 0.0 ) {
				w = sqrt( orb_el_buf.sm_axis ) * orb_el_buf.sm_axis;
				orb_el_buf.period = w;
				el_var[13].hyphen = 0;		/* period		*/
				if ( w > 0.0 ) {
					orb_el_buf.daily_motion =  DEG2RAD( 0.985607669 / w );
					el_var[12].hyphen = 0;	/* daily motion		*/
				}
			}
		}
		dim = ( ctrl_sw.ent_obj == COMET_IN ) ? 1 : 0;
		putval( &el_var[11], dim );		/* semi-major axis	*/
		putval( &el_var[12], dim );		/* daily motion		*/
		putval( &el_var[13], 1 );		/* period : dim		*/
	}

	return ( 0 );
}

static int	after_axis( dmy )
Field	*dmy;
{
	int		dim = 0;
	double	w;

	w = sqrt( orb_el_buf.sm_axis );
	if ( w > 0.0 ) {
		orb_el_buf.daily_motion = DEG2RAD( 0.985607669
										  / orb_el_buf.sm_axis / w );
		orb_el_buf.peri_dis = orb_el_buf.sm_axis * ( 1.0-orb_el_buf.ecc );
		orb_el_buf.time_pass = orb_el_buf.epoch
						  - orb_el_buf.mean_anomaly / orb_el_buf.daily_motion;
		orb_el_buf.period = w * orb_el_buf.sm_axis;
	}

	dim = ( ctrl_sw.ent_obj == ASTEROID_IN ) ? 1 : 0;
	putval( &el_var[ 5], dim );		/* time_pass	*/
	putval( &el_var[ 9], dim );		/* peri_dis		*/
	putval( &el_var[12], dim );		/* daily motion	*/
	putval( &el_var[13], dim );		/* period		*/

	return ( 0 );
}

static int	after_h1( dmy )
Field	*dmy;
{
	int		retval,	dim;

	if ( fabs(orb_el_buf.h1) < 0.0 ) {
		orb_el_buf.h2 = orb_el_buf.k1 = orb_el_buf.k2 = 0.0;
		dim = ( ctrl_sw.ent_obj == COMET_IN ) ? 0 : 1;
		putval( &el_var[15], dim );		/* k1	*/
		putval( &el_var[16], dim );		/* h2	*/
		putval( &el_var[17], dim );		/* k2	*/
	}

	return ( 0 );
}

int	ent_orbit()
{
	register int	i,	dim;
	int				x= 4,	y=TOP_LINE;
	int				result,	deflt;
	int				ik;

	top_bar( TB_ED_ORB_EL );
	clear_bottom( 0, Y_BIAS );
	frame_box(x-2, y-1, 76, 23);

	guidance( GUID_KEYIN );

	el_var[11].hyphen = 0;			/* semi-major axis	*/
	el_var[12].hyphen = 0;			/* daily motion		*/
	el_var[13].hyphen = 0;			/* period		*/
	if ( global_sw.el_buf_stat == EL_NOT_READY
	  || orb_el_buf.ecc >= 1.0 ) {
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			el_var[11].hyphen = HYPHEN( 5, 6, 6 ); /* semi-major axis	*/
			el_var[12].hyphen = HYPHEN( 5, 6, 6 ); /* daily motion	*/
			el_var[13].hyphen = HYPHEN( 5, 6, 6 ); /* period			*/
		}
	}

	ik = ctrl_sw.time_system;			/* save */
	ctrl_sw.time_system = ET_IN;		/* fake */

	for ( i=0; el_var[i].varptr != NULL; i++ ) {
		el_menu[i].exe_flg = flgval[i][ctrl_sw.ent_obj];	/* flg set */
		dim = ( el_menu[i].exe_flg == NOT_EXEC ) ? 1 : 0;
		el_var[i].changed = 0;
		putval( &(el_var[i]), dim );
	}
	result = deflt = 0;
	do {
		result = menu_man( el_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	ctrl_sw.time_system = ik;			  /* restore */

	for ( i=0; el_menu[i].title != NULL; i++ ) {
		if ( el_var[i].changed )
			arry_max = 0;	/* recalculation */
	}
	if ( strlen( orb_el_buf.name )
	   && orb_el_buf.peri_dis != 0.0 ) {
		global_sw.el_buf_stat = EL_READY;
	}

	clear_bottom( x-2, y-1 );
	top_bar( TB_EPH_SUB );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	ent_ra()
 *		RA & Decl & Range entry/edit routine
 *
 *	   Dec. 28, 1988		by S.Yamane
 *	Modified		by K.Osada ( Dec.19, 1989 )
 */
#define	RA_TOP		14
static Field	ra_var[] = {
	{ &current.ra,			VAL_RA,		41, RA_TOP,		15,
	   "%3dh %02dm %05.2lfs",
	   " ##h ##m ##.##s",
	   NULL,
	},
	{ &current.decl,		VAL_DECL,	41, RA_TOP+1,	15,
	   "%+3dd %02d' %04.1lf \"",
	   "+##d ##' ##.# \"",
	   NULL,
	},
	{ &current.range,		VAL_DECL,	41, RA_TOP+2,	15,
	   "%3dd %02d' %04.1lf \"",
	   "###d ##' ##.# \"",
	   NULL,
	},
	{ &current.mag_limit,	VAL_DOUBLE,	41, RA_TOP+3,	13,
	   "%13.1lf",
	   "         ##.#",
	   NULL,
	},
	{ &current.altitude,	VAL_DEGREE,	41, RA_TOP+4,	13,
	   "%+13.1lf",
	   "        +##.#",
	   NULL,
	},
	{  NULL,				0,			-1, -1,	-1,
	   NULL,
	   NULL,
	   NULL,
	},
} ;

static Menu	ra_menu[] = {
	{ AUTOEXEC,	'\0',	"R.A.        ",		getval,		&ra_var[0]	},
	{ AUTOEXEC,	'\0',	"Decl.       ",		getval,		&ra_var[1]	},
	{ AUTOEXEC,	'\0',	"Range [deg] ",		getval,		&ra_var[2]	},
	{ AUTOEXEC,	'\0',	"Mag. limit  ",		getval,		&ra_var[3]	},
	{ AUTOEXEC,	'\0',	"Min.altitude",		getval,		&ra_var[4]	},
	{ WAIT_KEY,	'\0',	"Upper-Menu", 		menu_upper,	NULL		},
	{ NOT_EXEC,	'\0',	NULL,				NULL,		NULL		},
} ;


int	ent_ra()
{
	register int	i;
	int			x=25,	y=RA_TOP;
	int			result,	deflt;
	int			ik;

	top_bar( TB_ED_RANGE );
	guidance( GUID_KEYIN );
	clear_bottom( x-2, y-1 );

	frame_box(x-2, y-1, 35, 8);

	for ( i=0; ra_var[i].varptr != NULL; i++ ) {
		putval( &ra_var[i], 0 );
		ra_var[i].changed = 0;
	}
	result = deflt = 0;
	do {
		result=menu_man( ra_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	ent_location()
 *		Longitude / Latitude / Height entry/edit routine
 *
 *	   Dec. 28, 1988		by S.Yamane
 */
#define	ENT_LOC_BASELINE	15

static Field	loc_var[] = {
	{ &current.longitude,	VAL_DEGREE,	45, ENT_LOC_BASELINE+1,	10,
	   "% 10.5lf",
	   "+###.#####",
	   NULL,
	},
	{ &current.latitude,	VAL_DEGREE,	45, ENT_LOC_BASELINE+2,	10,
	   " % 9.5lf",
	   " +##.#####",
	   NULL,
	},
	{ &current.height,		VAL_DOUBLE,	45, ENT_LOC_BASELINE+3,	10,
	   "%10.0lf",
	   "     +####",
	   NULL,
	},
	{  NULL,				0,			-1, -1,	-1,
	   NULL,
	   NULL,
	   NULL,
	},
} ;

static Menu	loc_menu[] = {
	{ AUTOEXEC,	'\0',	"Longitude [deg]",	getval,		&loc_var[0]	},
	{ AUTOEXEC,	'\0',	"Latitude  [deg]",	getval,		&loc_var[1]	},
	{ AUTOEXEC,	'\0',	"Height    [m]  ",	getval,		&loc_var[2]	},
	{ WAIT_KEY,	'\0',	"Upper-Menu",		menu_upper,	NULL		},
	{ NOT_EXEC,	'\0',	NULL,				NULL,		NULL		},
} ;


int	ent_location()
{
	register int	i;
	int			x = 25,	y = ENT_LOC_BASELINE+1;
	int			result,	deflt;
	int			ik;

	top_bar( TB_ED_LOCATION );
	guidance( GUID_KEYIN );
	clear_bottom( 0, y-2 );

	frame_box(x-2, y-1, 34, 6);

	for ( i = 0; loc_var[i].varptr != NULL; i++ ) {
		putval( &loc_var[i], 0 );
		loc_var[i].changed = 0;
	}
	result = deflt = 0;
	do {
		result=menu_man( loc_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	for ( i=0; loc_var[i].varptr != NULL; i++ ) {
		if ( loc_var[i].changed )
			arry_max = 0;
	}

	clear_bottom( x-2, y-1 );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	set_Metval()
 *		set parameter to output Metcalf const.
 *
 *	   Dec. 08, 1988		by S.Yamane
 *	   Dec. 03, 1990		by K.Osada
 */
static Field	Met_var[] = {
	{ &current.metcalf_p,	VAL_DOUBLE,	35, 11,	8,
	   "% 8.5lf",
	   "+#.#####",
	   NULL,
	},
	{  NULL,				0,			-1, -1,	-1,
	   NULL,
	   NULL,
	   NULL,
	},
} ;

int	put_Metval()
{
	putval( &Met_var[0], 0 );

	return ( 0 );
}

int	set_Metval()
{
	return ( getval( &Met_var[0] ) );
}

/* ***************  end of input.c  ***************** */
