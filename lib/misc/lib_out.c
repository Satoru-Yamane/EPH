/*
 *		lib_out.c :
 *			orbital element output for library search
 *
 *		Program : Coded by K.Osada
 *					Mar. 05, 1989
 *		Modified        by S.Yamane
 *					Mar. 10, 1989
 *		Modified        by K.Osada
 *		            Apr. 17, 1989
 *
 *	$Header: LIB_OUT.Cv  1.13  91/03/12 18:17:06  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<stdarg.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"filer.h"
#include	"ephcurs.h"


/*
 *	node2jd ---
 *
 *	Program : Coded by K.Osada
 *		Feb. 22,  1989
 *
 */
double node2jd(double yjd, double L)
{
	int		n = 100;
	double	jd,	jd_min,	jd_max,	jd0,	m;

	if ( yjd < 2000000.0 )
		n = 0;

	jd_min = yjd - 365.2568983 / 2.0;
	jd_max = jd_min + 365.2568983;

	do {
		jd = ( RAD2DEG(L) - 237.039 + 360.0 * n ) / 0.98560910 + 2400000.5;
		if (jd < jd_min)
			n++;
		if (jd > jd_max)
			n--;
	} while ( jd < jd_min || jd > jd_max );

	do {
		jd0 = jd;
		m = DEG2RAD(315.253 + 0.98560027 * (jd - 2400000.5));
		jd = (RAD2DEG(L) - 237.039 + 360.0 * n - 1.916 * sin(m)
			- 0.020 * sin(2.0 * m) ) / 0.98560910 + 2400000.5;
	} while ( fabs(jd - jd0) > 0.001 );

	return ( jd );
}


/*
 *	peri_ecl ---
 *
 *	Program : Coded by K.Osada
 *				Feb. 20,  1989
 */
static void	peri_ecl(Orbit_element orb_el, double *L, double *B)
{
	double	wk1,	wk2,	wk3,	wk4,	wk5;
	double	x,	y,	z;

	wk1 = sin(orb_el.peri);
	wk2 = cos(orb_el.peri);
	wk3 = sin(orb_el.node);
	wk4 = cos(orb_el.node);
	wk5 = wk1 * cos(orb_el.incl);

	x = wk2 * wk3 + wk5 * wk4;
	y = wk2 * wk4 - wk5 * wk3;
	z = wk1 * sin(orb_el.incl);

	*L = atan2(x, y);
	*B = atan2(z, hypot(x, y));
}


/*
 *	p_item ---
 *	itemiaze output routine
 */
void
p_item( char *title, char *fmt, ... )
{
	va_list		arg_ptr;
	char		buf[200];

	bold_start();
	dos_printf( title );
	bold_end();

	underline_start();

	va_start( arg_ptr, fmt );
	vsprintf( buf, fmt, arg_ptr );
	dos_puts( buf );
	va_end( arg_ptr );

	underline_end();
}


/*
 *	output routine for Comet
 */
static int	comet_lib_out( n, dmy1, orb_el, cm_buf )
int		n,	dmy1;
Orbit_element	orb_el;
Comment		cm_buf;
{
	int		node_type;
	double	el_day,	jd1,	jd2,	rec_sm_axis;
	double	asc_r,	desc_r;
	Time	tm_el;
	Eph_const	eph_c;
	double	L,	B;

	vector_c( orb_el, &eph_c );

	clear_bottom( 0, Y_BIAS );

	p_item( "Rec.No:", "%4d\n", n+1 );
	reverse_start();
	dos_printf( "%s ",   orb_el.name  );
	dos_printf( "%s ",   orb_el.roman );
	dos_printf( "(%s)", orb_el.number );
	reverse_end();

	bold_start();
	if (ctrl_sw.orb_el_system==B1950_IN) {
		dos_puts( "           [Equinox:1950.0]\n" );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		dos_puts( "           [Equinox:2000.0]\n" );
	}
	bold_end();

	el_day = jd2day(orb_el.time_pass, &tm_el);
	p_item( "Perihelion time                    (T): ",
			"%4d %s%8.5lf", tm_el.year, mon2strmon(tm_el.mon), el_day );

	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET\nPerihelion distance                (q): ",
				"%11.7lf", orb_el.peri_dis );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT\nPerihelion distance                (q): ",
				"%11.7lf", orb_el.peri_dis );
	}
	p_item( " AU           Px=", "%+10.8lf\n", eph_c.Px );

	p_item( "Eccentricity                       (e): ",
			"%11.7lf", orb_el.ecc );
	p_item( "              Py=", "%+10.8lf\n", eph_c.Py );

	p_item( "Revolution period                  (P): ",
			( orb_el.ecc < 1.0 && orb_el.period<999.99 ) ?
			"%6.2lf" :  "   -  ",
			orb_el.period );
	p_item( " years             Pz=", "%+10.8lf\n", eph_c.Pz );

	p_item( "Argument of perihelion         (Peri.): ",
			"%9.5lf", RAD2DEG( orb_el.peri ) );
	p_item( "                Qx=", "%+10.8lf\n", eph_c.Qx );

	p_item( "Longitude of the ascending node (Node): ",
			"%9.5lf", RAD2DEG( orb_el.node ) );
	p_item( "                Qy=", "%+10.8lf\n", eph_c.Qy );

	p_item( "Inclination                    (Incl.): ",
			"%9.5lf", RAD2DEG( orb_el.incl ) );
	p_item( "                Qz=", "%+10.8lf\n", eph_c.Qz );

	if ( orb_el.epoch != 0.0 ) {
		el_day = jd2day(orb_el.epoch, &tm_el);
		p_item( "Osculation epoch               (Epoch): ",
				"%4d %s%4.1lf",
				tm_el.year, mon2strmon(tm_el.mon), el_day );
		if (ctrl_sw.orb_el_system==B1950_IN) {
			p_item( " ET\n", "" );
		} else if (ctrl_sw.orb_el_system==J2000_IN) {
			p_item( " TT\n", "" );
		}
	} else {
	    p_item( "Osculation epoch               (Epoch): ", "   -\n" );
	}

	if ( orb_el.ecc < 1.0 ) {
		p_item( "Semi-major axis                    (a): ",
				"%11.7lf", orb_el.sm_axis );
		p_item( " AU\nMean daily motion                  (n): ",
				"%12.8lf\n", RAD2DEG( orb_el.daily_motion ) );
		p_item( "Aphelion distance                  (Q): ",
				"%11.7lf", orb_el.sm_axis * (orb_el.ecc + 1.0) );
		p_item( " AU\n", "" );
	} else {
		p_item( "Semi-major axis                    (a): ", "   -\n" );
		p_item( "Mean daily motion                  (n): ", "   -\n" );
		p_item( "Aphelion distance                  (Q): ", "   -\n" );
	}
	if (orb_el.ecc != 1.0) {
		rec_sm_axis = (1.0 - orb_el.ecc) / orb_el.peri_dis;
		p_item( "Reciprocal semimajor axis        (1/a): ",
				"%+11.7lf", rec_sm_axis );
		p_item( " /AU\n", "" );
	} else {
		p_item( "Reciprocal semimajor axis        (1/a): ", "   -\n" );
	}
	peri_ecl( orb_el, &L, &B );
	if (L < 0.0)
		L += PI;
	p_item( "Longitude of perihelion            (L): ",
			"%6.2lf\n", RAD2DEG( L ) );
	p_item( "Latitude of perihelion             (B): ",
			"%+6.2lf\n", RAD2DEG( B ) );

	jd1 = node2jd( orb_el.time_pass, orb_el.node + PI );
	el_day = jd2day( jd1, &tm_el );
	if (orb_el.ecc == 1.0) {
		asc_r = orb_el.peri_dis / cos(orb_el.peri/2.0) / cos(orb_el.peri/2.0);
	} else {
		asc_r = orb_el.sm_axis * (1.0 - orb_el.ecc * orb_el.ecc)
								/ (1.0 + orb_el.ecc * cos(orb_el.peri));
	}
	p_item( "Ascending node                  (date): ",
			"%4d %s%5.2lf",
			tm_el.year, mon2strmon(tm_el.mon), el_day );
	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET         r:", "%6.3lf\n", asc_r );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT         r:", "%6.3lf\n", asc_r );
	}

	jd2 = node2jd( orb_el.time_pass, orb_el.node );
	el_day = jd2day( jd2, &tm_el );
	if (orb_el.ecc == 1.0) {
		desc_r = orb_el.peri_dis / sin(orb_el.peri/2.0) / sin(orb_el.peri/2.0);
	} else {
		desc_r = orb_el.sm_axis *  (1.0 - orb_el.ecc * orb_el.ecc)
				/ (1.0 - orb_el.ecc * cos(orb_el.peri) );
	}
	p_item( "Descending node                 (date): ",
			"%4d %s%5.2lf",
			tm_el.year, mon2strmon(tm_el.mon), el_day );
	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET         r:", "%6.3lf\n", desc_r );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT         r:", "%6.3lf\n", desc_r );
	}

	p_item( "Magnitude parameters                H1:", "%5.2lf", orb_el.h1 );
	p_item( "  K1:", "%5.2lf", orb_el.k1 );
	p_item( "  H2:", "%5.2lf", orb_el.h2 );
	p_item( "  K2:", "%5.2lf\n",  orb_el.k2 );

	p_item( "Number of observations:", "%4d", cm_buf.n_obs );
	p_item( "  Arc:", "%s", cm_buf.arc );
	p_item( "  Number of planets:", "%2d\n", cm_buf.n_perterb_pla );
	p_item( "[Note]\n    ", "%s", cm_buf.note );

	return ( 0 );
}



static int	mpla_lib_out( n, dmy1, orb_el, cm_buf )
int		n,	dmy1;
Orbit_element	orb_el;
Comment		cm_buf;
{
	int		node_type;
	double		el_day,	jd1,	jd2,	asc_r,	desc_r;
	Time		tm_el;
	Eph_const	eph_c;
	double		L,	B;

	vector_c( orb_el, &eph_c );

	clear_bottom( 0, Y_BIAS );

	p_item( "Rec.No:", "%4d\n", n+1 );

	reverse_start();
	dos_printf( "(%-7.7s) ", orb_el.number );
	dos_printf( "%s", orb_el.name );
	reverse_end();

	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( "                                      [Equinox:1950.0]\n", "" );
	} else if( ctrl_sw.orb_el_system==J2000_IN) {
		p_item( "                                      [Equinox:2000.0]\n", "" );
	}
	el_day = jd2day(orb_el.epoch, &tm_el);
	p_item( "Osculation epoch               (Epoch): ",
			"%4d %s%4.1lf", tm_el.year, mon2strmon(tm_el.mon), el_day );
	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET\n", "" );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT\n", "" );
	}

	if (orb_el.g == 0.0) {
		p_item( "Absolute magnitude              B(1,0): ",
				"%5.2lf", orb_el.h2 );
		p_item( "                ", "" );
	} else {
		p_item( "Absolute mag. & Slope parameter    (H): ",
				"%5.2lf", orb_el.h2 );
		p_item( "      (G): ", "%5.2lf", orb_el.g );
	}
	p_item( "    Px=", "%+10.8lf\n", eph_c.Px );

	p_item( "Mean anomaly                       (M): ",
			"%9.5lf", RAD2DEG(orb_el.mean_anomaly) );
	p_item( "                Py=", "%+10.8lf\n", eph_c.Py );

	p_item( "Argument of perihelion         (Peri.): ",
			"%9.5lf", RAD2DEG(orb_el.peri) );
	p_item( "                Pz=", "%+10.8lf\n", eph_c.Pz );

	p_item( "Longitude of the ascending node (Node): ",
			"%9.5lf", RAD2DEG(orb_el.node) );
	p_item( "                Qx=", "%+10.8lf\n", eph_c.Qx );

	p_item( "Inclination                    (Incl.): ",
			"%9.5lf", RAD2DEG(orb_el.incl) );
	p_item( "                Qy=", "%+10.8lf\n", eph_c.Qy );

	p_item( "Eccentricity                       (e): ",
			"%11.7lf", orb_el.ecc );
	p_item( "              Qz=", "%+10.8lf\n", eph_c.Qz );

	p_item( "Mean daily motion                  (n): ",
			"%12.8lf\n", RAD2DEG( orb_el.daily_motion ) );

	p_item( "Semi-major axis                    (a): ",
			"%11.7lf", orb_el.sm_axis );

	orb_el.time_pass = orb_el.epoch - orb_el.mean_anomaly/orb_el.daily_motion;
	el_day = jd2day(orb_el.time_pass, &tm_el);
	p_item( " AU\nPerihelion time                    (T): ",
			"%4d %s%8.5lf", tm_el.year, mon2strmon(tm_el.mon), el_day );

	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET\nPerihelion distance                (q): ",
				"%11.7lf", orb_el.peri_dis );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT\nPerihelion distance                (q): ",
				"%11.7lf", orb_el.peri_dis );
	}

	p_item( " AU\nAphelion distance                  (Q): ",
			"%11.7lf", orb_el.sm_axis * ( orb_el.ecc + 1.0 ) );

	p_item( " AU\nRevolution period                  (P): ",
			"%6.2lf", orb_el.period);

	peri_ecl( orb_el, &L, &B );
	if (L < 0.0)
		L += PI;
	p_item( " years\nLongitude of perihelion            (L): ",
			"%6.2lf\n", RAD2DEG( L ) );
	p_item( "Latitude of perihelion             (B): ",
			"%+6.2lf\n", RAD2DEG( B ) );

	jd1 = node2jd( orb_el.time_pass, orb_el.node );
	el_day = jd2day(jd1, &tm_el);
	asc_r = orb_el.sm_axis * (1.0 - orb_el.ecc * orb_el.ecc)
			/ (1.0 + orb_el.ecc * cos(orb_el.peri));
	p_item( "Ascending node                  (date): ",
			"%4d %s%5.2lf",
			tm_el.year, mon2strmon(tm_el.mon), el_day );
	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET         r:", "%6.3lf\n", asc_r );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT         r:", "%6.3lf\n", asc_r );
	}

	jd2 = node2jd( orb_el.time_pass, orb_el.node + PI );
	el_day = jd2day(jd2, &tm_el);
	desc_r = orb_el.sm_axis * ( 1.0 - orb_el.ecc * orb_el.ecc ) 
			/ (1.0 - orb_el.ecc * cos(orb_el.peri) );
	p_item( "Descending node                 (date): ",
			"%4d %s%5.2lf",
			tm_el.year, mon2strmon(tm_el.mon), el_day );
	if (ctrl_sw.orb_el_system==B1950_IN) {
		p_item( " ET         r:", "%6.3lf\n", desc_r );
	} else if (ctrl_sw.orb_el_system==J2000_IN) {
		p_item( " TT         r:", "%6.3lf\n", desc_r );
	}

	p_item( "[Note]\n    ", "%s\n", cm_buf.note );

	return ( 0 );
}


/*
 *	filer cmd
 */
#if 0
extern int	meteor_forecast();
extern int	change_year();
#endif

static Filer_cmd	filer_cmd[] = {
	{  TB_LIB_SEARCH,	is_live,	comet_lib_out,		NULL	},
	{  TB_LIB_SEARCH,	is_live,	mpla_lib_out,		NULL	},
/*	{  TB_LIB_SEARCH,	is_live,	meteor_forecast,	change_year	}, */
} ;

static MenuFunc	lib_out()
{
	if ( ctrl_sw.ent_obj == COMET_IN ) {
		scan_file( &filer_cmd[0] );
	} else {
		scan_file( &filer_cmd[1] );
	}

	return ( 0 );
}

#if 0
static MenuFunc	meteor_out()
{
	scan_file( &filer_cmd[2] );

	return ( 0 );
}
#endif

/*
 *	menu item
 */
extern MenuFunc	edit_lib_mask( void );

static Menu	lib_menus[] = {
/*	{ WAIT_KEY,	'E',	"Edit Range       ",	edit_lib_mask,	NULL  }, */
	{ WAIT_KEY,	'S',	"Search           ",	lib_out,		NULL  },
/*	{ WAIT_KEY,	'M',	"Meteor Forecast  ",	meteor_out,		NULL  }, */
	{ WAIT_KEY,	'B',	"Back to Main Menu",	menu_upper,		NULL  },
	{ NOT_EXEC,	'\0',	NULL,					NULL,			NULL  },
};

/*
 *	lib_menu ---
 *		Menu for Library search
 */
int
lib_menu()
{
	int		w_mode;
	int		deflt,	result;

	w_mode = global_sw.exec_mode;
	global_sw.exec_mode = EXEC_LIB;
	if ( ctrl_sw.ent_obj == ASTEROID_IN )
		lib_menus[2].exe_flg =  NOT_EXEC;
	else
		lib_menus[2].exe_flg =  WAIT_KEY;

	top_bar( TB_LIB_SEARCH );
	guidance( GUID_SUB );

	clear_bottom( 25, Y_BIAS+5 );
	result = deflt = 0;
	do {
		result = menu_man( lib_menus, 25, Y_BIAS+3, MN_SELECT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;
	global_sw.exec_mode = w_mode;

	return ( menu_upper() );
}

/* ****************  end of lib_out.c  ******************* */
