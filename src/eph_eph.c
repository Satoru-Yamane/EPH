/*
 *	eph_eph.c :
 *		Ephemeris Menu for EPH
 *
 *	(C)opyright 1989,  all right reserved
 *		Jan.06, 1989	  by S.Yamane
 *  					     K.Osada
 *
 *	    Apr.24, 1989         K.Osada
 *	    Dec.20, 1989	     K.Osada
 *
 *	$Header: EPH_EPH.Cv  1.16  91/01/17 15:26:08  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"filer.h"
#include	"ephcurs.h"

#define		NEARLY_ZERO		1.0e-8

#define		FAKE			1		/* fake line control in case IAUC style */

/*
 *	functions
 *	   &
 *	menu data
 */
static Eph_result	calc_1_day(double jde, Eph_const eph_c)
{
	Xyz		d_xyz;		/* parallax correction */
	Lbr		lbr;		/* use R-value only	*/
	double	delta0,		delta;
	double	obj_jde,	interval;
	double	r_cosv,		r_sinv,		r_obj;
	double	ra,		decl;
	static Eph_result	result;

	if ( ctrl_sw.opt_sw ) {
		jde = optimum( ( (long)jde - 0.5 ), orb_el_buf,
						 current.longitude, current.latitude );
	}
	result.jde = jde;
	if ( ctrl_sw.simple_sw ) {
		newcombs( jde, &(result.xyz), &lbr );
	} else {
		newcomb1( jde, &(result.xyz), &lbr );
	}
	result.lbr_r = lbr.r;
	if ( ctrl_sw.parallax_sw ) {		/*  correct parallax */
		parallax( current.longitude, current.latitude, current.height,
				  jde, &d_xyz );
		result.xyz.x += d_xyz.x;
		result.xyz.y += d_xyz.y;
		result.xyz.z += d_xyz.z;
		result.lbr_r = sqrt( result.xyz.x*result.xyz.x
							+ result.xyz.y*result.xyz.y
							+ result.xyz.z*result.xyz.z );
	}

	delta = 1.5;
	do {
		delta0 = delta;
		obj_jde = jde - LIGHT_TIME * delta;
		interval = obj_jde - orb_el_buf.time_pass;
		if ( orb_el_buf.ecc>0.95 ) {
			n_parabola( orb_el_buf,interval, &r_cosv, &r_sinv, &r_obj );
		} else {
			elipse( orb_el_buf,interval, &r_cosv, &r_sinv, &r_obj );
		}
		obj_pos( eph_c, r_cosv, r_sinv, result.xyz, &(result.pos) );
		locate( result.pos, &ra, &decl, &delta );
	} while ( fabs( delta - delta0 ) > NEARLY_ZERO );

	result.delta = delta;
	result.r_obj = r_obj;

	return ( result );
}


/*
 *	eph_calc_main()
 *
 */
static MenuFunc	eph_calc_main()
{
	register int	i;
	int				ik;
	int				data_top_line,	line_now;
	int				data_lines,	head_lines,	el_lines;
#if FAKE
	int				fake_data_lines;
#endif
	int				count,	more_cnt,	use_more;
	double			jde;
	Eph_const		eph_c;
	Eph_result		today,	tomorrow;
	FILE			*outfp;
	char			msg[ 50 ];

	if ( global_sw.el_buf_stat == EL_NOT_READY ) {
		warning( "Element NOT ready!" );
		return ( 0 );
	}
	if ( ctrl_sw.micro_sw && ctrl_sw.simple_sw ) {
		ik = warning( "Mismatch accuracy. Continue?(y/n)" );
		if ( ik == 'n' || ik == 'N' )
			return ( 0 );
	}
	top_bar( TB_EPH_CALC );
	guidance( GUID_CALC );
	clear_bottom( 0, Y_BIAS );

	switch ( ctrl_sw.eph_out ) {
	case DEV_CONSOLE:
			outfp = stderr;
			break;
	case DEV_PRINTER:
			outfp = stdout;		/* please use pipe */
			break;
	case DEV_FILE:
			outfp = fopen( current.w_eph_file, "ab" );
			if ( outfp == NULL ) {
				warning( "Output to stderr" );
				outfp = stderr;
				ctrl_sw.eph_out = DEV_CONSOLE;
			}
	}

	moveto( 0, Y_BIAS );
	data_top_line = Y_BIAS;
	el_lines = head_lines = 0;

	if ( ctrl_sw.orbital_sw )
		el_lines = orb_el_out( &orb_el_buf, &comment, outfp );
	head_lines += header1( outfp );
	head_lines += header2( outfp );
	line_now = data_top_line + el_lines + head_lines;

	if ( ctrl_sw.fix_elem )
		data_top_line += ( el_lines + head_lines );
	else
		data_top_line += head_lines;

	vector_c( orb_el_buf, &eph_c );

	if ( ctrl_sw.eph_mode == 1 ) {		/* Single date mode */
		current.end_jd = current.start_jd;
	}

	count = 0;
	data_lines = head_lines;
	more_cnt = EPH_lines - line_now;
	use_more = ctrl_sw.use_more_eph;	/* may be changed */
	for ( jde = current.start_jd; jde <= current.end_jd; jde += current.step ) {

		if ( count >= arry_max ) {			/* Not calculated yet */
			today = calc_1_day( jde, eph_c );
			if ( count < arry_size ) {		/* Check buffer overflow */
				eph_result[ count ] = today;
				arry_max++;
			}
		} else {
			today = eph_result[ count ];
		}
		/*
		 * Pre-calculate for Daily-motion or NEXT step
		 */
		if ( count+1 >= arry_max ) {		/* Not calculated yet */
			tomorrow = calc_1_day( jde+current.step, eph_c );
			if ( count+1 < arry_size ) {	/* Check buffer overflow */
				eph_result[ count+1 ] = tomorrow;
				arry_max++;
			}
		} else {
			tomorrow = eph_result[ count+1 ];
		}
		/*
		 *	output 1 line
		 */
	 	if ( line_now > EPH_lines - data_lines - 1 ) {
			if ( ! ctrl_sw.fix_elem && el_lines > 0 ) {
				el_lines -= data_lines;
				scroll_up( Y_BIAS, data_lines );
			} else {
				scroll_up( data_top_line, data_lines );
			}
			line_now -= data_lines;
		}
		moveto(0, line_now);
		data_lines = eph_out( count, today, tomorrow, eph_c, outfp );
		if ( ! ctrl_sw.fix_elem && el_lines && el_lines < data_lines ) {
			scroll_up( Y_BIAS, el_lines );
			line_now -= el_lines;
			el_lines = 0;
		}
		line_now += data_lines;
		more_cnt -= data_lines;
#if FAKE
		if ( ctrl_sw.IAUC_sw ) {
			if ( count == 0 ) {			/* save first line length */
				fake_data_lines = data_lines;
			}
			if ( count % 2 == 0 )		/* next line (odd) is 1 line */
				data_lines = 1;
			else
				data_lines = fake_data_lines;
		}
#endif

		count++;

		/*
		 *	stop scroll and wait command
		 */
		if ( use_more && more_cnt <= data_lines ) {
			int	cmd = more_cmd( GUID_CALC );

			if ( cmd == MORE_1_LINE ) {
				more_cnt += data_lines;
			} else if ( cmd == MORE_1_PAGE ) {
				more_cnt = EPH_lines - data_top_line;
			} else if ( cmd == MORE_ALL ) {
				use_more = 0;
			} else if ( cmd == MORE_QUIT ) {
				break;
			}
		}
	}
	fflush( outfp );
	if ( ! ( outfp == stderr || outfp == stdout ) )
		fclose( outfp );

	sprintf( msg, "Output %d ephemerides", count );
	top_bar( TB_EPH_CALC );
	guidance( GUID_PAUSE );
	pause( msg );

	guidance( GUID_SUB );
	clear_bottom( 0, Y_BIAS );

	return ( 0 );
}



/*
 *	Select output item or output device/format
 *		forced to re-calculate
 */
#define	EPH_CTRL_X0				15
#define	EPH_CTRL_X1				40
#define	EPH_CTRL_BASELINE		3

static Select	sys_sw[] = {
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+1, 14, &ctrl_sw.orbital_sw,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+2, 14, &ctrl_sw.comment_sw,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+3, 14,	&ctrl_sw.simple_sw,
		{ "Standard",	"Simple",	NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+4, 14,	&ctrl_sw.micro_sw,
		{ "0.1\'",	"0.1\"",	NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+5, 14,	&ctrl_sw.parallax_sw,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+6, 14,	&ctrl_sw.date_opt_sw,
		{ "Manual",	"Auto",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+7, 14,	&ctrl_sw.opt_sw,
		{ "Manual",	"Auto",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+8, 14,	&ctrl_sw.IAUC_sw,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+9, 14,	&ctrl_sw.eph_out_mode,
		{ "Normal",	NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+10, 14,	&ctrl_sw.use_more_eph,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+11, 14,	&ctrl_sw.fix_elem,
		{ "No",		"Yes",		NULL }
	},
	{ EPH_CTRL_X1, EPH_CTRL_BASELINE+12,  9,	&ctrl_sw.eph_out,
		{ "Console",	"Printer",	"File",		NULL }
	},
	{ -1,		-1,			-1,	NULL,
		{ NULL }
	},
} ;

extern MenuFunc	sel_dev();

static Menu	eph_con_menu[] = {
	{ AUTOEXEC,	'\0',	"Orbital elem. output",	eph_select,	&(sys_sw[ 0])	},
	{ AUTOEXEC,	'\0',	"Comment field output",	eph_select,	&(sys_sw[ 1])	},
	{ AUTOEXEC,	'\0',	"Calculation accuracy",	eph_select,	&(sys_sw[ 2])	},
	{ AUTOEXEC,	'\0',	"Output accuracy     ",	eph_select,	&(sys_sw[ 3])	},
	{ AUTOEXEC,	'\0',	"Correct Parallax    ",	eph_select,	&(sys_sw[ 4])	},
	{ AUTOEXEC,	'\0',	"Optimum obs. date   ",	eph_select,	&(sys_sw[ 5])	},
	{ AUTOEXEC,	'\0',	"Optimum obs. time   ",	eph_select,	&(sys_sw[ 6])	},
	{ AUTOEXEC,	'\0',	"IAUC style          ",	eph_select,	&(sys_sw[ 7])	},
	{ AUTOEXEC,	'\0',	"Output mode         ",	eph_select,	&(sys_sw[ 8])	},
	{ AUTOEXEC,	'\0',	"Stop Scroll(more)   ",	eph_select,	&(sys_sw[ 9])	},
	{ AUTOEXEC,	'\0',	"Fixed orbital elem. ",	eph_select,	&(sys_sw[10])	},
	{ AUTOEXEC,	'\0',	"Output device       ",	sel_dev,	&(sys_sw[11])	},
	{ WAIT_KEY,	'\0',	"Upper-Menu          ",	menu_upper,	NULL	},
	{ NOT_EXEC,	'\0',	NULL,			NULL,		NULL	},
} ;

static MenuFunc	eph_o_ctl()
{
	register int	i;
	int		x = EPH_CTRL_X0,	y = Y_BIAS+EPH_CTRL_BASELINE+1;
	int		deflt,	result;
	int		w_simp,	w_prec,	w_date_opt, w_opt, w_palla;

	top_bar( TB_EPH_O_CTL );
	guidance( GUID_YESNO );
	clear_bottom( 0, y-2 );
	frame_box(x-2, y-1, 52, 15);

	for ( i = 0; sys_sw[i].sw_var != NULL; i++ ) {
		if ( eph_con_menu[i].exe_flg == AUTOEXEC ) {
			disp_val( &(sys_sw[i]) );
		}
	}
	w_simp     = ctrl_sw.simple_sw;
	w_prec     = ctrl_sw.micro_sw;
	w_date_opt = ctrl_sw.date_opt_sw;
	w_opt      = ctrl_sw.opt_sw;
	w_palla    = ctrl_sw.parallax_sw;
	result     = deflt = 0;
	do {
		result = menu_man( eph_con_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
		if ( ctrl_sw.simple_sw && ctrl_sw.micro_sw ) {
			warning( "Mismatch accuracy." );
			result &= ~ ( UPPER_MENU | QUIT );
			deflt = 0;
		}
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	if (  w_simp       != ctrl_sw.simple_sw
	   || w_prec       != ctrl_sw.micro_sw
	   || w_date_opt   != ctrl_sw.date_opt_sw
	   || w_opt        != ctrl_sw.opt_sw
	   || w_palla      != ctrl_sw.parallax_sw ) {
		arry_max = 0;		/* precision or Optimum time changed */
	}				/* So, force to re-calculate */

	clear_bottom( x-2, y-1 );
	top_bar( TB_EPH_O_CTL );
	guidance( GUID_SUB );

	return ( 0 );
}



/*
 *	select output item - I
 *		output from eph buffer
 *
 */
#define		I1BASE			1
static Select	item_sw[] = {
	{ 45,	I1BASE+0,	8,	&ctrl_sw.B1950_OUT,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+1,	8,	&ctrl_sw.J2000_OUT,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+2,	8,	&ctrl_sw.DATE_OUT,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+3,	8,	&ctrl_sw.delta_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+4,	8,	&ctrl_sw.radius_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+5,	8,	&ctrl_sw.d_mot_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+6,	8,	&ctrl_sw.mag1_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+7,	8,	&ctrl_sw.mag2_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+8,	8,	&ctrl_sw.vmag_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+9,	8,	&ctrl_sw.elong_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+10,	8,	&ctrl_sw.height_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+11,	8,	&ctrl_sw.tail_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+12,	8,	&ctrl_sw.phase_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+13,	8,	&ctrl_sw.moon_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+14,	8,	&ctrl_sw.moon_light_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+15,	8,	&ctrl_sw.mot_pa_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45,	I1BASE+16,	8,	&ctrl_sw.var_T_sw,
		{ "No",		"Yes",		NULL }
	},
	{ -1,	-1,			-1,	NULL,		{ NULL }	},
} ;


static Menu	eph_item_menu[] = {
	{ AUTOEXEC,	'\0',	"Equinox      (B1950)",	eph_select,		&(item_sw[ 0])	},
	{ AUTOEXEC,	'\0',	"Equinox      (J2000)",	eph_select,		&(item_sw[ 1])	},
	{ AUTOEXEC,	'\0',	"Equinox      (DATE )",	eph_select,		&(item_sw[ 2])	},
	{ AUTOEXEC,	'\0',	"Delta               ",	eph_select,		&(item_sw[ 3])	},
	{ AUTOEXEC,	'\0',	"Radius              ",	eph_select,		&(item_sw[ 4])	},
	{ AUTOEXEC,	'\0',	"Dairy motion        ",	eph_select,		&(item_sw[ 5])	},
	{ AUTOEXEC,	'\0',	"m1 (for comet)      ",	eph_select,		&(item_sw[ 6])	},
	{ AUTOEXEC,	'\0',	"m2 (for comet)      ",	eph_select,		&(item_sw[ 7])	},
	{ NOT_EXEC,	'\0',	"V/B  (for mi.planet)",	eph_select,		&(item_sw[ 8])	},
	{ AUTOEXEC,	'\0',	"Elongation          ",	eph_select,		&(item_sw[ 9])	},
	{ AUTOEXEC,	'\0',	"Altitude & Azimuth  ",	eph_select,		&(item_sw[10])	},
	{ AUTOEXEC,	'\0',	"Tail                ",	eph_select,		&(item_sw[11])	},
	{ AUTOEXEC,	'\0',	"Phase               ",	eph_select,		&(item_sw[12])	},
	{ AUTOEXEC,	'\0',	"Moon age            ",	eph_select,		&(item_sw[13])	},
	{ AUTOEXEC,	'\0',	"Moon light          ",	eph_select,		&(item_sw[14])	},
	{ AUTOEXEC,	'\0',	"Motion & Pos.angle  ",	eph_select,		&(item_sw[15])	},
	{ AUTOEXEC,	'\0',	"Variation           ",	eph_select,		&(item_sw[16])	},
	{ WAIT_KEY,	'\0',	"Upper-Menu          ",	menu_upper,	NULL	},
	{ NOT_EXEC,	'\0',	NULL,					NULL,		NULL	},
} ;



static void	set_flg(void)
{
	if ( ctrl_sw.ent_obj == COMET_IN ) {
		eph_item_menu[  6 ].exe_flg = AUTOEXEC;		/* m1   */
		eph_item_menu[  7 ].exe_flg = AUTOEXEC;		/* m2   */
		eph_item_menu[  8 ].exe_flg = NOT_EXEC;		/* V/B  */
		eph_item_menu[ 11 ].exe_flg = AUTOEXEC;		/* tail */
/*		eph_item_menu[ 16 ].exe_flg = AUTOEXEC;	*/	/* var T */
	} else {
		eph_item_menu[  6 ].exe_flg = NOT_EXEC;
		eph_item_menu[  7 ].exe_flg = NOT_EXEC;
		eph_item_menu[  8 ].exe_flg = AUTOEXEC;
		eph_item_menu[ 11 ].exe_flg = NOT_EXEC;
/*		eph_item_menu[ 16 ].exe_flg = NOT_EXEC; */
	}
}

static MenuFunc	eph_item()
{
	register int	i;
	int			x = 15,	y = I1BASE+Y_BIAS;
	int			deflt,	result;

	top_bar( TB_EPH_O_CTL );
	guidance( GUID_YESNO );
	clear_bottom( 0, Y_BIAS );
	frame_box(x-2, y-1, 47, 20);

	set_flg();
	for ( i = 0; item_sw[i].sw_var != NULL; i++ ) {
		if ( eph_item_menu[i].exe_flg == AUTOEXEC ) {
			disp_val( &(item_sw[i]) );
		}
	}
	result = deflt = 0;
	do {
		result = menu_man( eph_item_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	top_bar( TB_EPH_O_CTL );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	select output item - II
 *		output from eph buffer
 *
 */
static Select	item_sw2[] = {
	{ 45, 14, 8,	&ctrl_sw.metcalf_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, 15, 8,	&ctrl_sw.sun2obj_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, 16, 8,	&ctrl_sw.e2t_sw,
		{ "No",		"Yes",		NULL }
	},
	{ -1, -1, -1,	NULL,		{ NULL }	},
} ;

static Menu	eph_item_menu2[] = {
	{ AUTOEXEC,	'\0',	"Constants of Metcalf",	eph_select,		&(item_sw2[0])	},
	{ AUTOEXEC,	'\0',	"SUN B/L from object ",	eph_select,		&(item_sw2[1])	},
	{ AUTOEXEC,	'\0',	"Earth - tail angle  ",	eph_select,		&(item_sw2[2])	},
	{ WAIT_KEY,	'\0',	"Upper-Menu          ",	menu_upper,	NULL	},
	{ NOT_EXEC,	'\0',	NULL,					NULL,		NULL	},
} ;

static void	set_flg2()
{
	if ( ctrl_sw.ent_obj == COMET_IN ) {
		eph_item_menu2[ 2 ].exe_flg = AUTOEXEC;		/* E - tail */
	} else {
		eph_item_menu2[ 2 ].exe_flg = NOT_EXEC;
	}
}

static MenuFunc	eph_item2()
{
	register int	i;
	int			x = 15,	y = Y_BIAS+14;
	int			deflt,	result;

	top_bar( TB_EPH_O_CTL );
	guidance( GUID_YESNO );
	clear_bottom( x-2, y-2 );
	frame_box(x-2, y-1, 47, 6);

	set_flg();
	for ( i = 0; item_sw2[i].sw_var != NULL; i++ ) {
		if ( eph_item_menu2[i].exe_flg == AUTOEXEC ) {
			disp_val( &(item_sw2[i]) );
		}
	}
	result = deflt = 0;
	do {
		result = menu_man( eph_item_menu2, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	top_bar( TB_EPH_O_CTL );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	select eph date
 *		output from eph buffer
 *
 */
#define	EPH_D_Y0		13

static Select	eph_date_sw[] = {
	{ 45, EPH_D_Y0+0, 10,	&ctrl_sw.jd_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, EPH_D_Y0+1, 10,	&ctrl_sw.mjd_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, EPH_D_Y0+2, 10,	&ctrl_sw.year_sw,
		{ "Header",	"Line",		NULL }
	},
	{ 45, EPH_D_Y0+3, 10,	&ctrl_sw.month_sw,
		{ "Num",	"Char",		NULL }
	},
	{ 45, EPH_D_Y0+4, 10,	&ctrl_sw.day_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, EPH_D_Y0+5, 10,	&ctrl_sw.week_sw,
		{ "No",		"Yes",		NULL }
	},
	{ 45, EPH_D_Y0+6, 10,	&ctrl_sw.time_sw,
		{ "Header",	"Line",		NULL }
	},
	{ -1, -1, -1,	NULL,
		{ NULL }
	},
} ;

static Menu	eph_date_menu[] = {
	{ AUTOEXEC,	'\0',	"Julian day         ",	eph_select,		&(eph_date_sw[0])	},
	{ AUTOEXEC,	'\0',	"Modified julian day",	eph_select,		&(eph_date_sw[1])	},
	{ AUTOEXEC,	'\0',	"Year               ",	eph_select,		&(eph_date_sw[2])	},
	{ AUTOEXEC,	'\0',	"Type of month      ",	eph_select,		&(eph_date_sw[3])	},
	{ AUTOEXEC,	'\0',	"Day                ",	eph_select,		&(eph_date_sw[4])	},
	{ AUTOEXEC,	'\0',	"Day of week        ",	eph_select,		&(eph_date_sw[5])	},
	{ AUTOEXEC,	'\0',	"Time               ",	eph_select,		&(eph_date_sw[6])	},
	{ WAIT_KEY,	'\0',	"Upper-Menu         ",	menu_upper,	NULL	},
	{ NOT_EXEC,	'\0',	NULL,					NULL,		NULL	},
} ;

static MenuFunc	eph_date()
{
	register int	i;
	int			x = 15,	y = Y_BIAS + EPH_D_Y0;
	int			deflt,	result;

	top_bar( TB_EPH_O_CTL );
	guidance( GUID_YESNO );
	clear_bottom( x-2, y-2 );
	frame_box(x-2, y-1, 49, 10);

	for ( i = 0; eph_date_sw[i].sw_var != NULL; i++ ) {
		if ( eph_date_menu[i].exe_flg == AUTOEXEC ) {
			disp_val( &(eph_date_sw[i]) );
		}
	}
	result = deflt = 0;
	do {
		result = menu_man( eph_date_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;

	clear_bottom( x-2, y-1 );
	top_bar( TB_EPH_O_CTL );
	guidance( GUID_SUB );

	return ( 0 );
}


/*
 *	Ephemeris sub-menu
 */
static Filer_cmd	funcs = {	/* cmd packet for scan_file() */
	TB_LOAD_ELEMENT,		/* top bar level	*/
	is_live,				/* mask function	*/
	display_element,		/* display function	*/
	copy_to_calc_buf,		/* when CR pressed	*/
} ;

extern MenuFunc	ent_orbit();
extern MenuFunc	ent_date();
extern MenuFunc	ent_location();
extern MenuFunc	eph_calc_main();
extern MenuFunc	eph_o_ctl();
extern MenuFunc	eph_date();
extern MenuFunc	eph_item();
extern MenuFunc	eph_item2();

static Menu	eph_submenu[] = {
	{ WAIT_KEY,	'F',	"Load elem. from File ",	scan_file,		&funcs  },
	{ WAIT_KEY,	'E',	"Edit Orbit Element   ",	ent_orbit,		NULL  },
	{ WAIT_KEY,	'R',	"Edit Date Range      ",	ent_date,		NULL  },
	{ WAIT_KEY,	'L',	"Edit Location        ",	ent_location,	NULL  },
	{ WAIT_KEY,	'C',	"Calculation          ",	eph_calc_main,	NULL  },
	{ WAIT_KEY,	'D',	"Select output date   ",	eph_date,		NULL  },
	{ WAIT_KEY,	'1',	"Select output item 1 ",	eph_item,		NULL  },
	{ WAIT_KEY,	'2',	"Select output item 2 ",	eph_item2,		NULL  },
	{ WAIT_KEY,	'O',	"Output control       ",	eph_o_ctl,		NULL  },
	{ WAIT_KEY,	'B',	"Back to Main Menu    ",	menu_upper,		NULL  },
	{ NOT_EXEC,	'\0',	NULL,						NULL,			NULL  },
};


/*
 *	eph_menu ---
 *		Ephemeris menu
 */
MenuFunc
eph_menu()
{
	int		w_mode;
	int		deflt,	result;

	w_mode = global_sw.exec_mode;
	global_sw.exec_mode = EXEC_EPH;
	clear_bottom( 25, Y_BIAS+5 );
	result = deflt = 0;
	do {
		top_bar( TB_EPH_SUB );
		guidance( GUID_SUB );
		result = menu_man( eph_submenu, 25, Y_BIAS+2, MN_SELECT, deflt);
		deflt = ITEM_NO( result );
	} while ( ! ( result & UPPER_MENU || result & QUIT ) ) ;
	global_sw.exec_mode = w_mode;

	return ( menu_upper() );
}
/* ***************  end of eph_eph.c  ****************** */
