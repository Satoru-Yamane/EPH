/*
 *	eph_id.c :
 *		Identification Menu for EPH
 *
 *	(C)opyright 1989,  all right reserved
 *		Jan.06, 1989	by S.Yamane
 *						   K.Osada
 *
 *	    Apr.16, 1989       K.Osada
 *	    Dec.21, 1989	   K.Osada
 *
 *	$Header: EPH_ID.Cv  1.12  91/01/17 15:25:44  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<string.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"filer.h"
#include	"ephcurs.h"

extern void	daily_m();

static MenuFunc	id_calc()
{
	register int	id_cnt;
	int			recno,	recsize;
	int			data_top_line,	line_now,	data_lines,	more_cnt,	use_more;
	double		id_jde;
	double		r_obj,	delta,	pos_angle;
	double		ra,	decl, ra_date, decl_date, d_ra, d_decl;
	double		oc_ra,	oc_decl,	mag;
	double		distance,	phase,	azimuth,	altitude;
	double		w1,	w2;
	static Orbit_element	orb_el;
	static Comment	cm_buf;
	Eph_const		eph_c;
	Xyz			xyz1950,	d_xyz;
	Lbr			lbr;
	Xyz			pos,	pos1;
	char		wk[ FN_LEN ];
	FILE		*fp,	*fpo;

	top_bar( TB_ID_CALC );
	if ( current.range == 0.0 ) {
		warning( "Please enter range." );
		return ( 0 );
	}
	/* ***** prepare output stream ***** */
	switch ( ctrl_sw.id_out ) {
	case DEV_CONSOLE:
			fpo = stderr;
			break;
	case DEV_PRINTER:
			fpo = stdout;		/* please use pipe */
			break;
	case DEV_FILE:
			strcpy( wk, current.w_id_file );
			fpo = fopen( wk, "ab" );
			if ( fpo == NULL ) {
				warning( "Open error!! Output to stderr." );
				fpo = stderr;
				ctrl_sw.id_out = 0;
			}
			break;
	default:
			fpo = stderr;
	}

	/* ***** prepare input stream ***** */
	fp = eph_fopen( &recsize );
	if ( fp == NULL ) {
		warning( "Input file open error!!" );
		return ( 0 );
	}

	guidance( GUID_CALC );
	clear_bottom( 0, Y_BIAS );

	moveto( 0, Y_BIAS );

	id_jde = current.id_start_jd;

	line_now = data_top_line = Y_BIAS + id_header( id_jde, fpo );
	more_cnt = EPH_lines - data_top_line - 1;
	newcomb1( id_jde, &xyz1950, &lbr );

	parallax( current.longitude, current.latitude, current.height, id_jde,
			 &d_xyz );
	xyz1950.x += d_xyz.x;		/*  correct parallax */
   	xyz1950.y += d_xyz.y;
   	xyz1950.z += d_xyz.z;

	data_lines = 0;
	use_more = ctrl_sw.use_more_id;	/* may be changed */
	for ( id_cnt = recno = 0; ; recno++ ) {

		if ( load_element( fp, recno, recsize, &orb_el, &cm_buf ) )
			break;
		vector_c( orb_el, &eph_c );
		eph_calc( id_jde, orb_el, eph_c, xyz1950,
				  &pos, &ra, &decl, &delta, &r_obj );
		ra_decl( pos, &ra, &decl );

		oc_ra   = in_pi( current.ra - ra );
		oc_decl = current.decl - decl;

		/*
		 *	check distance
		 */
		lenth_pa( current.ra, current.decl, ra, decl, &distance, &phase );
		if ( distance > current.range )
			continue;

		/*
		 *	check altitude
		 */
		prec_b( JD1950, id_jde, pos, &pos1 );
		ra_decl( pos1, &ra_date, &decl_date );
		equ2hor( id_jde, current.longitude, current.latitude,
		 		 ra_date, decl_date, &azimuth, &altitude );
		if ( altitude < current.altitude )
			continue;

		if ( ctrl_sw.ent_obj == COMET_IN ) {		/* m1 for comet */
			mag = comet_mag1( orb_el.h1, orb_el.k1, delta, r_obj );
		} else {					/* magnitude for minor planet */
			lbr.r = sqrt(xyz1950.x*xyz1950.x+xyz1950.y*xyz1950.y+xyz1950.z*xyz1950.z);
			pos_angle = position_angle( lbr.r, r_obj, delta );
			if ( orb_el.g == 0.0 ) {
				output.mag2 = mp_v_mag( orb_el.h2, delta, r_obj, pos_angle );
			} else {
				mag = mp_p_mag( orb_el.h2, orb_el.g,
								 delta, r_obj, pos_angle );
			}
		}
		/*
		 *	check magnitude
		 */
		if ( mag > current.mag_limit )
			continue;

		/*
		 *	output 1 line
		 */
	 	if ( line_now >= EPH_lines - data_lines ) {		/* data_lines==0 at 1st */
			scroll_up( data_top_line, data_lines );
		}

		switch ( ctrl_sw.id_out_mode ) {
		case ID_O_HA:			/* h-A mode */
				data_lines = id_out( fpo,
							orb_el.roman, orb_el.number, orb_el.name,
							ra, decl, azimuth, altitude, mag );
				line_now += data_lines;
				break;
		case ID_O_OC:			/* O-C mode */
				data_lines = oc_out( fpo,
							orb_el.roman, orb_el.number, orb_el.name,
							oc_ra, oc_decl, distance, mag );
				line_now += data_lines;
				break;
		}

		id_cnt++;

		if ( use_more && ( more_cnt -= data_lines ) <= 0 ) {
			int	cmd = more_cmd( GUID_CALC );

			if ( cmd == MORE_1_LINE ) {
				more_cnt += data_lines;
			} else if ( cmd == MORE_1_PAGE ) {
				more_cnt = EPH_lines - data_top_line - 1;
			} else if ( cmd == MORE_ALL ) {
				use_more = 0;
			} else if ( cmd == MORE_QUIT ) {
				break;
			}
		}
	}
	fclose( fp );

 	if ( line_now >= EPH_lines - data_lines ) {		/* data_lines==0 at 1st */
		scroll_up( data_top_line, data_lines );
	}
	id_trailer( fpo );
	fflush( fpo );
	if ( fpo != stderr  &&  fpo != stdout ) {
		fclose( fpo );
	}

	top_bar( TB_ID_CALC );
	sprintf( wk, "Output %d ", id_cnt );
	strcat( wk, ( ctrl_sw.ent_obj == COMET_IN ? "Comets" : "Astroids" ) );
	guidance( GUID_PAUSE );
	pause( wk );
	clear_bottom( 0, Y_BIAS );
	guidance( GUID_SUB );

	return ( 0 );
}


#define	ID_CTRL_BASELINE		11

static Select	sys_sw[] = {
	{ 40, ID_CTRL_BASELINE+1,  7,	&ctrl_sw.id_out_mode,
		{ "h/A",	"O-C",	NULL }
	},
	{ 40, ID_CTRL_BASELINE+2,  7,	&ctrl_sw.use_more_id,
		{ "No",		"Yes",	NULL }
	},
	{ 40, ID_CTRL_BASELINE+3,  9,	&ctrl_sw.id_out,
		{ "Console",	"Printer",	"File",		NULL }
	},
	{ -1, -1, -1,	NULL,
		{ NULL }
	},
} ;

extern MenuFunc	sel_dev();

static Menu	id_con_menu[] = {
	{ AUTOEXEC,	'O',	"Output mode  ",	eph_select,	&(sys_sw[0])	},
	{ AUTOEXEC,	'S',	"Stop scroll  ",	eph_select,	&(sys_sw[1])	},
	{ AUTOEXEC,	'D',	"Output device",	sel_dev,	&(sys_sw[2])	},
	{ WAIT_KEY,	'U',	"Upper-menu   ",	menu_upper,	NULL		},
	{ NOT_EXEC,	'\0',	NULL,			NULL,		NULL		},
} ;


static MenuFunc	id_o_ctl()
{
	register Select	*sp;
	int		x = 20,	y = Y_BIAS+ID_CTRL_BASELINE+1;
	int		deflt,	rslt;

	top_bar( TB_ID_O_CTL );
	guidance( GUID_YESNO );
	clear_bottom( x-2, y-1 );
	frame_box(x-2, y-1, 50, 6);

	for ( sp = sys_sw; sp->sw_var != NULL; sp++ ) {
		disp_val( sp );
	}
	rslt = deflt = 0;
	do {
		rslt = menu_man( id_con_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( rslt );
	} while ( !( rslt & UPPER_MENU || rslt & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	top_bar( TB_ID_O_CTL );
	guidance( GUID_SUB );

	return ( 0 );
}


extern MenuFunc	ent_id_date();
extern MenuFunc	ent_ra();
extern MenuFunc	ent_location();
extern MenuFunc	id_o_ctl();

static Menu	id_submenu[] = {
	{ WAIT_KEY,	'D',	"Edit Current Date    ",	ent_id_date,	NULL  },
	{ WAIT_KEY,	'R',	"Edit Current Range   ",	ent_ra,			NULL  },
	{ WAIT_KEY,	'L',	"Edit Current Location",	ent_location,	NULL  },
	{ WAIT_KEY,	'C',	"Calculation          ",	id_calc,		NULL  },
	{ WAIT_KEY,	'O',	"Output control       ",	id_o_ctl,		NULL  },
	{ WAIT_KEY,	'B',	"Back to Main Menu    ",	menu_upper,		NULL  },
	{ NOT_EXEC,	'\0',	NULL,						NULL,			NULL  },
};


/*
 *	id_menu ---
 *		Identification main menu
 */
MenuFunc
id_menu()
{
	int		w_mode;
	int		deflt,	result;

	w_mode = global_sw.exec_mode;
	global_sw.exec_mode = EXEC_IDENT;
	clear_bottom( 25, Y_BIAS+5 );
	result = deflt = 0;
	do {
		top_bar( TB_ID_SUB );
		guidance( GUID_SUB );
		result = menu_man( id_submenu, 25, Y_BIAS+3, MN_SELECT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;
	global_sw.exec_mode = w_mode;

	return ( menu_upper() );
}

/* **************  end of eph_id.c  ************* */
