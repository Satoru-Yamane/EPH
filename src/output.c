/*
 *	output.c
 *		EPH output routine
 *
 *	(C)opyright 1989,  all right reserved
 *	   Apr. 16, 1989	by K.Osada
 *						   S.Yamane
 *		 May  19, 1989
 *		 May  24, 1989
 *		 Nov  19, 1989
 *		 Dec  21, 1989
 *		 Dec  09, 1990
 *
 *	$Header: OUTPUT.Cv  1.11  91/01/25 14:37:56  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#include	<stdarg.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"ephcurs.h"

/*
 *	Output routine for Ephemeris
 *
 *	 by	S.Yamane
 *		K.Osada
 */

static FILE	*out_fp2 = NULL;			/* secondary output stream */

static int	eph_putc( c )
int	c;
{
	dos_putc( c );
	if ( out_fp2 != NULL && out_fp2 != stderr && out_fp2 != stdout )
		fputc( c, out_fp2 );

	return ( c );
}

static int	eph_puts(char *s)
{
	if ( out_fp2 != NULL && out_fp2 != stderr && out_fp2 != stdout )
		fputs( s, out_fp2 );

	return ( dos_puts( s ) );
}


static int	eph_printf( char *fmt, ... )
{
	char		buf[500];
	va_list		arg_ptr;

	va_start( arg_ptr, fmt );
	vsprintf( buf, fmt, arg_ptr );
	va_end( arg_ptr );

	return ( eph_puts( buf ) );
}


/*
 *	level 1
 */
int
header1( fp )
FILE	*fp;
{
	register int	chars = 0,	hd_lines = 0;
	Time			tm_p1,	tm_p2;

	out_fp2 = fp;

	if ( ctrl_sw.date_opt_sw == 1 ) {		/* optimum date mode */
		current.start_jd = (long)(orb_el_buf.time_pass - 365.2425/2.0) + 0.5;
		current.end_jd = (long)(orb_el_buf.time_pass + 365.2425/2.0) + 0.5;
	}

	if ( ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {	/* NORMAL format */
		chars = 0;
		if ( ctrl_sw.jd_sw ) {
			chars += eph_printf( "JD            " );
		}
		if ( ctrl_sw.mjd_sw ) {
			chars += eph_printf( "MJD         " );
		}
		if ( ctrl_sw.micro_sw && ctrl_sw.simple_sw == 0 ) {
			if ( ctrl_sw.year_sw ) {
				chars += eph_printf( "     " );
			}
			if ( ctrl_sw.month_sw ) {
				chars += eph_printf( "  " );
			}
			if ( ctrl_sw.day_sw ) {
				if ( !ctrl_sw.year_sw ) {
					jd2day( time_convert(current.start_jd), &tm_p1 );
					jd2day( time_convert(current.end_jd), &tm_p2 );
					if ( tm_p1.year == tm_p2.year ) {
						chars += eph_printf( "%4d     ", tm_p1.year );
					} else {
						chars += eph_printf( "%4d/%2d  ", tm_p1.year,
							tm_p2.year - tm_p2.year/100*100 );
					}
				} else {
					chars += eph_printf( "Date     " );
				}
			} else {
				chars += eph_printf( " " );
			}
			if ( ctrl_sw.week_sw ) {
				chars += eph_printf( "    " );
			}
			if ( ctrl_sw.time_sw ) {
				switch ( ctrl_sw.time_system ) {
				case TT_IN:		chars += eph_printf( "(TT)   " );		break;
				case ET_IN:		chars += eph_printf( "(ET)   " );		break;
				case UT_IN:		chars += eph_printf( "(UT)   " );		break;
				case JST_IN:	chars += eph_printf( "(JST)  " );		break;
				}
			}
			if ( ctrl_sw.B1950_OUT )
				chars += eph_printf( " R.A.   (1950)   Decl.   " );
			if ( ctrl_sw.J2000_OUT )
				chars += eph_printf( " R.A.   (2000)   Decl.   " );
			if ( ctrl_sw.DATE_OUT  )
				chars += eph_printf( " R.A.   (date)   Decl.   " );
		} else {
			if ( ctrl_sw.year_sw ) {
				chars += eph_printf( "     " );
			}
			if ( ctrl_sw.month_sw ) {
				chars += eph_printf( "  " );
			}
			if ( ctrl_sw.day_sw ) {
				if ( !ctrl_sw.year_sw ) {
					jd2day( time_convert(current.start_jd), &tm_p1 );
					jd2day( time_convert(current.end_jd), &tm_p2 );
					if ( tm_p1.year == tm_p2.year ) {
						chars += eph_printf( "%4d   ", tm_p1.year );
					} else {
						chars += eph_printf( "%4d/%2d", tm_p1.year,
							tm_p2.year - tm_p2.year/100*100 );
					}
				} else {
					chars += eph_printf( "Date   " );
				}
			} else {
				chars += eph_printf( " " );
			}
			if ( ctrl_sw.week_sw ) {
				chars += eph_printf( "    " );
			}
			if ( ctrl_sw.time_sw ) {
				switch ( ctrl_sw.time_system ) {
				case TT_IN:		chars += eph_printf( "(TT)  " );		break;
				case ET_IN:		chars += eph_printf( "(ET)  " );		break;
				case UT_IN:		chars += eph_printf( "(UT)  " );		break;
				case JST_IN:	chars += eph_printf( "(JST) " );		break;
				}
			}
			if ( ctrl_sw.B1950_OUT )
				chars += eph_printf( " R.A.(1950) Decl.  " );
			if ( ctrl_sw.J2000_OUT )
				chars += eph_printf( " R.A.(2000) Decl.  " );
			if ( ctrl_sw.DATE_OUT  )
				chars += eph_printf( " R.A.(date) Decl.  " );
		}
		if ( ctrl_sw.delta_sw )
			chars += eph_printf( " Delta " );
		if ( ctrl_sw.radius_sw )
			chars += eph_printf( "   r   " );
		if ( ctrl_sw.d_mot_sw )
			chars += eph_printf( " Daily motion " );
		if ( ctrl_sw.var_T_sw )
			chars += eph_printf( "  Variation   " );
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			if ( ctrl_sw.mag1_sw )
				chars += eph_printf( "  m1  " );
			if ( ctrl_sw.mag2_sw )
				chars += eph_printf( "  m2  " );
		} else if ( ctrl_sw.ent_obj == ASTEROID_IN ) {
			if ( ctrl_sw.vmag_sw ) {
				if ( orb_el_buf.g == 0.0 )
					chars += eph_printf( "  B   " );
				else
					chars += eph_printf( "  V   " );
			}
		}
		if ( ctrl_sw.mot_pa_sw )
			chars += eph_printf( " Mot./PA  " );
		if ( ctrl_sw.elong_sw )
			chars += eph_printf( " Elong." );
		if ( ctrl_sw.phase_sw )
			chars += eph_printf( "    P  " );
		if ( ctrl_sw.tail_sw )
			chars += eph_printf( "  PA/Tail " );
		if ( ctrl_sw.height_sw )
			chars += eph_printf( "    h/A     " );
		if ( ctrl_sw.moon_sw )
			chars += eph_printf( " Moon Age" );
		if ( ctrl_sw.moon_light_sw )
			chars += eph_printf( "  Moon h/A  " );
		if ( ctrl_sw.metcalf_sw )
			chars += eph_printf( " Metcalf(M/PA)" );
		if ( ctrl_sw.sun2obj_sw )
			chars += eph_printf( "    Lo/Bo    " );
		if ( ctrl_sw.e2t_sw )
			chars += eph_printf( "E-tail"  );
	}
	eph_printf( "\n");
	hd_lines += ( --chars / EPH_cols + 1 );

	return ( hd_lines );
}


int
header2( fp )
FILE	*fp;
{
	register int	chars = 0,	hd_lines = 0;
	double			jd,	wk;
	Time			tm_p;

	out_fp2 = fp;

	if ( ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {	/* NORMAL format */
		if ( ctrl_sw.jd_sw ) {
			chars += eph_printf( "              " );
		}
		if ( ctrl_sw.mjd_sw ) {
			chars += eph_printf( "            " );
		}
		if ( ctrl_sw.year_sw ) {
			chars += eph_printf( "     " );
		}
		if ( ctrl_sw.month_sw ) {
			chars += eph_printf( "  " );
		}
		if ( ctrl_sw.day_sw ) {
			if ( modf(current.step, &wk)==0.0
					 && !ctrl_sw.time_sw && !ctrl_sw.opt_sw ) {
					jd2day( time_convert(current.start_jd), &tm_p );
					chars += eph_printf( "%02dh%02dm ",
									tm_p.hour, tm_p.min );
			} else {
				chars += eph_printf( "       " );
			}
		} else {
			chars += eph_printf( " " );
		}
		if ( ctrl_sw.week_sw ) {
			chars += eph_printf( "    " );
		}
		if ( ctrl_sw.micro_sw && ctrl_sw.simple_sw == 0 ) {
			if ( ctrl_sw.time_sw ) {
				chars += eph_printf( " h  m  s " );
			}
			if ( ctrl_sw.B1950_OUT )
				chars += eph_printf( "  h  m  s      o  '  \"   " );
			if ( ctrl_sw.J2000_OUT )
				chars += eph_printf( "  h  m  s      o  '  \"   " );
			if ( ctrl_sw.DATE_OUT )
				chars += eph_printf( "  h  m  s      o  '  \"   " );
		} else {
			if ( ctrl_sw.time_sw ) {
				chars += eph_printf( " h  m " );
			}
			if ( ctrl_sw.B1950_OUT )
				chars += eph_printf( "  h  m      o  '   " );
			if ( ctrl_sw.J2000_OUT )
				chars += eph_printf( "  h  m      o  '   " );
			if ( ctrl_sw.DATE_OUT )
				chars += eph_printf( "  h  m      o  '   " );
		}
		if ( ctrl_sw.delta_sw )
			chars += eph_printf( "       " );
		if ( ctrl_sw.radius_sw )
			chars += eph_printf( "       " );
		if ( ctrl_sw.d_mot_sw )
			chars += eph_printf( "   m       '  " );
		if ( ctrl_sw.var_T_sw ) {
			if ( ctrl_sw.ent_obj == COMET_IN )
	 			chars += eph_printf( " for T=+1 day " );
			else if ( ctrl_sw.ent_obj == ASTEROID_IN )
	 			chars += eph_printf( " for M=+1 deg " );
		}
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			if ( ctrl_sw.mag1_sw )
				chars += eph_printf( "      " );
			if ( ctrl_sw.mag2_sw )
				chars += eph_printf( "      " );
		}
		if ( ctrl_sw.vmag_sw && ctrl_sw.ent_obj == ASTEROID_IN )
			chars += eph_printf( "      " );
		if ( ctrl_sw.mot_pa_sw )
			chars += eph_printf( "   '     o" );
		if ( ctrl_sw.elong_sw )
			chars += eph_printf( "    o  " );
		if ( ctrl_sw.phase_sw )
			chars += eph_printf( "    o  " );
		if ( ctrl_sw.tail_sw )
			chars += eph_printf( "    o o   " );
		if ( ctrl_sw.height_sw )
			chars += eph_printf( "   o     o  " );
		if ( ctrl_sw.moon_sw )
			chars += eph_printf( "    o    " );
		if ( ctrl_sw.moon_light_sw )
			chars += eph_printf( "   o     o  " );
		if ( ctrl_sw.metcalf_sw )
			chars += eph_printf( "    s     o   " );
		if ( ctrl_sw.sun2obj_sw )
			chars += eph_printf( "    o     o  " );
		if ( ctrl_sw.e2t_sw )
			chars += eph_printf( "   o  "  );
	}
	eph_printf( "\n");
	hd_lines += ( --chars / EPH_cols + 1 );

	return ( hd_lines );
}

/*
 *  orbital elements print
 */
int
orb_el_out( element, cmnt, fp )
Orbit_element	*element;
Comment			*cmnt;
FILE	*fp;
{
	register int	chars,	o_lines;
	double	el_day;
	Time	tm_el;

	out_fp2 = fp;

	chars = o_lines = 0;
	if ( fp == stderr || ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {
		if ( ctrl_sw.ent_obj == COMET_IN ) {		/* comet routine */
			chars += eph_printf( "          " );
			chars += eph_printf( "%s ",	element->name );
			chars += eph_printf( "%s ",	element->roman );
			chars += eph_printf( "(%s)\n\n", element->number ) - 2;
			o_lines += ( --chars / EPH_cols + 2 );
			chars = 0;

			el_day = jd2day( element->time_pass, &tm_el );
			chars += eph_printf( "          " );
			chars += eph_printf( "T     = %4d ",	tm_el.year );
			chars += eph_printf( "%s",		mon2strmon(tm_el.mon) );
			if (ctrl_sw.orb_el_system==B1950_IN) {
				chars += eph_printf( "%8.5lf ET",		el_day ) - 1;
			} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
				chars += eph_printf( "%8.5lf TT",		el_day ) - 1;
			}
			el_day = jd2day( element->epoch, &tm_el );
			chars += eph_printf( "    " );
			chars += eph_printf( "Epoch = %4d ",	tm_el.year );
			chars += eph_printf( "%s",		mon2strmon(tm_el.mon) );
			if (ctrl_sw.orb_el_system==B1950_IN) {
				chars += eph_printf( "%4.1lf ET\n",		el_day ) - 1;
			} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
				chars += eph_printf( "%4.1lf TT\n",		el_day ) - 1;
			}
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "          " );
			chars += eph_printf( "Peri. = %9.5lf",	RAD2DEG( element->peri ) );
			chars += eph_printf( "                " );
			chars += eph_printf( "e = %11.7lf\n",	element->ecc ) - 1;
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "          " );
			chars += eph_printf( "Node  = %9.5lf",	RAD2DEG( element->node ) );
			if ( ctrl_sw.orb_el_system == B1950_IN ) {
				chars += eph_printf( "   1950.0" );
			} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
				chars += eph_printf( "   2000.0" );
			}
			if ( element->ecc < 1.0 ) {
				chars += eph_printf( "       a = %11.7lf AU",
												element->sm_axis );
			}
			eph_printf( "\n" );
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "          " );
			chars += eph_printf( "Incl. = %9.5lf",	RAD2DEG(element->incl) );
			if ( element->ecc < 1.0 ) {
				chars += eph_printf( "                " );
				chars += eph_printf( "n = %12.8lf",
									RAD2DEG(element->daily_motion) );
			}
			eph_printf( "\n" );
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "          q     = %11.7lf AU",
													element->peri_dis );
			if ( element->ecc < 1.0 ) {
				chars += eph_printf( "           " );
				chars += eph_printf( "P =%7.2lf years", element->period );
			}
			eph_printf( "\n\n" );
			o_lines += ( --chars / EPH_cols + 2 );
			chars = 0;

			if ( ctrl_sw.mag1_sw ) {
				chars += eph_printf( "          " );
				chars += eph_printf( "m1 = %5.2lf",		element->h1 );
				chars += eph_printf( " + 5log(delta) + " );
				chars += eph_printf( "%5.2lflog(r)\n", element->k1 ) - 1;
				o_lines += ( --chars / EPH_cols + 1 );
				chars = 0;
			}
			if ( ctrl_sw.mag2_sw ) {
				chars += eph_printf( "          " );
				chars += eph_printf( "m2 = %5.2lf",		element->h2 );
				chars += eph_printf( " + 5log(delta) + %5.2lf",	element->k2 );
				chars += eph_printf( "log(r) + 0.03(phase)\n" ) - 1;
				o_lines += ( --chars / EPH_cols + 1 );
				chars = 0;
			}
			if ( ctrl_sw.comment_sw ) {
				eph_printf( "\nNote:\n" );
				o_lines += 2;
				chars += eph_printf( "     %s\n", cmnt->note ) - 1;
				o_lines += ( --chars / EPH_cols + 1 );
				chars = 0;
			}
		} else {			/* Minor planet */
			chars += eph_printf( "          " );
			chars += eph_printf( "(%-7.7s)  %s\n\n",
									 element->number, element->name ) - 2;
			o_lines += ( --chars / EPH_cols + 2 );
			chars = 0;

			el_day = jd2day( element->epoch, &tm_el );
			chars += eph_printf( "          " );
			chars += eph_printf( "Epoch = %4d ",	tm_el.year );
			chars += eph_printf( "%s",		mon2strmon(tm_el.mon) );
			chars += eph_printf( "%4.1lf ET\n          ",		el_day ) - 1;
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "Peri. = %9.5lf                ",
										  RAD2DEG( element->peri ) );
			chars += eph_printf( "e = %11.7lf\n          ", element->ecc ) - 1;
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			if ( ctrl_sw.orb_el_system == B1950_IN ) {
				chars += eph_printf( "Node  = %9.5lf   1950.0       ",
										  RAD2DEG( element->node ) );
			} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
				chars += eph_printf( "Node  = %9.5lf   2000.0       ",
										  RAD2DEG( element->node ) );
			}
			chars += eph_printf( "a = %11.7lf AU    \n          ",
										  element->sm_axis ) - 1;
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "Incl. = %9.5lf                ",
										  RAD2DEG( element->incl ) );
			chars += eph_printf( "n = %12.8lf\n          ",
										  RAD2DEG( element->daily_motion ) ) - 1;
			o_lines += ( --chars / EPH_cols + 1 );
			chars = 0;

			chars += eph_printf( "M     = %9.5lf                ",
										  RAD2DEG( element->mean_anomaly ) );
			chars += eph_printf( "P = %6.2lf years\n\n",
									element->period	 ) - 2;
			o_lines += ( --chars / EPH_cols + 2 );
			chars = 0;

			if ( ctrl_sw.vmag_sw ) {
				if ( element->g == 0.0 ) {
					chars += eph_printf( "          " );
					chars += eph_printf( "B = %5.2lf",		element->h2 );
					chars += eph_printf( " + 5log(delta) + 5log(r)" );
					chars += eph_printf( " + 0.023(phase)\n" ) - 1;
				} else {
					chars += eph_printf( "          " );
					chars += eph_printf( "H =%6.2lf    ", element->h2 );
					chars += eph_printf( "G =%6.2lf\n", element->g ) - 1;
				}
				o_lines += ( --chars / EPH_cols + 1 );
				chars = 0;
			}
			if ( ctrl_sw.comment_sw ) {
				eph_printf( "\nNote:\n" );
				o_lines += 2;
				chars += eph_printf( "     %s\n", cmnt->note ) - 1;
				o_lines += ( --chars / EPH_cols + 1 );
				chars = 0;
			}
		}
		eph_printf( "\n\n" );
		o_lines += 2;
	}

	return ( o_lines );
}


int eqpos_out(double ra, double decl, FILE *outfp)
{
	register int	chars = 0;
	int				sg_ra,		ra_h,		ra_min;
	double			ra_sec;
	int				sg_decl,	decl_deg,	decl_min;
	double			decl_sec;

	out_fp2 = outfp;

	ra = in_pix2( ra );

	sg_ra   = ra2hms(  ra,   &ra_h,	 &ra_min,   &ra_sec  );
	sg_decl = rad2dms( decl, &decl_deg, &decl_min, &decl_sec );
	if ( ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {		/* NORMAL format */
		if ( ctrl_sw.micro_sw && ctrl_sw.simple_sw == 0 ) {
			if ( fabs(decl) < DEG2RAD(70.0) ) {
				chars += eph_printf( " %02d %02d %05.2lf",
									ra_h, ra_min, ra_sec );
			} else {
				chars += eph_printf( " %02d %02d %04.1lf ",
									ra_h, ra_min, ra_sec );
			}
			chars += eph_printf( " %c",  sg_decl>0 ? '+' : '-' );
			chars += eph_printf( "%02d %02d %04.1lf ",
									decl_deg, decl_min, decl_sec );
		} else {
			if ( fabs(decl) < DEG2RAD(70.0) ) {
				chars += eph_printf( " %02d %05.2lf",  ra_h, ra_min+ra_sec/60.0 );
			} else {
				chars += eph_printf( " %02d %04.1lf ",  ra_h, ra_min+ra_sec/60.0 );
			}
			chars += eph_printf( " %c",  sg_decl>0 ? '+' : '-' );
			chars += eph_printf( "%02d %04.1lf ", decl_deg, decl_min+decl_sec/60.0 );
		}
	}

	return ( chars );
}

static int	time_out(Eph_result eph_r, FILE	 *outfp)
{
	register int	chars = 0;
	double			jd;
	Time			tm_p;

	out_fp2 = outfp;

	jd = time_convert( eph_r.jde );

/* correct of ET-UT interval start_jd to jd for output times */
	if (( ctrl_sw.time_system == JST_IN || ctrl_sw.time_system == UT_IN) && ctrl_sw.ut2et_sw == 0) {
		jd += ( ut2et(jd) - ut2et(current.start_jd) );
	}

	jd2day( jd, &tm_p );

	if ( ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {		/* NORMAL format */
		if ( ctrl_sw.jd_sw ) {
			chars += eph_printf( "%12.5lf ", eph_r.jde );
		}
		if ( ctrl_sw.mjd_sw ) {
			chars += eph_printf( "%10.5lf ", eph_r.jde - 2400000.5 );
		}
		if ( ctrl_sw.micro_sw && ctrl_sw.simple_sw == 0 ) {
			if ( ctrl_sw.year_sw ) {
				chars += eph_printf( "%4d ", tm_p.year );
			}
			if ( ctrl_sw.day_sw ) {
				if ( ctrl_sw.month_sw ) {
					chars += eph_printf( "%s%2d ",
										mon2strmon(tm_p.mon), tm_p.day );
				} else {
					chars += eph_printf( "%2d/%2d ", tm_p.mon, tm_p.day );
				}
			}
			if ( ctrl_sw.week_sw ) {
				chars += eph_printf( "%3s ", day_of_week( jd ) );
			}
			if ( ctrl_sw.time_sw ) {
				chars += eph_printf( "%02d %02d %02.0lf ",
								tm_p.hour, tm_p.min, tm_p.csec/100.0 );
			}
		} else {
			if ( ctrl_sw.year_sw ) {
				chars += eph_printf( "%4d ", tm_p.year );
			}
			if ( ctrl_sw.day_sw ) {
				if ( ctrl_sw.month_sw ) {
					chars += eph_printf( "%s%2d ",
										mon2strmon(tm_p.mon), tm_p.day );
				} else {
					chars += eph_printf( "%2d/%2d ", tm_p.mon, tm_p.day );
				}
			}
			if ( ctrl_sw.week_sw ) {
				chars += eph_printf( "%3s ", day_of_week( jd ) );
			}
			if ( ctrl_sw.time_sw ) {
				chars += eph_printf( "%02d %02d ",
								tm_p.hour, tm_p.min );
			}
		}
	}

	return ( chars );
}


int
eph_out( n, eph_today, eph_tomorrow, eph_c, outfp )
int		n;
Eph_result	eph_today,	eph_tomorrow;
Eph_const	eph_c;
FILE		*outfp;
{
	register int	chars = 0;
	double			jde0,	jde;
	double			ra,	decl;
	double			ra1,	decl1;
	double			wk_ra,	wk_decl;
	double			ra_date,	decl_date;
	double			T_ra,	T_decl;
	double			mot,	pa;
	double			moon_ra, moon_decl, sin_pai, moon_elong, moon_pa;
	double			Bo,	Lo;
	double			Hx,	Hy,	Hz,	metcalf_mot, theta;
	double			wk,	wk1,	wk2,	wk3,	w1;
	Xyz				xyz;
	Xyz				pos0, pos1, pos_date;

	out_fp2 = outfp;

	jde = eph_today.jde;
	chars += time_out( eph_today, outfp );

	xyz = eph_today.xyz;	/* it must be done for Daily motion */
	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		pos1 = eph_today.pos;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		Xyz	xyz_i,	xyz_o;

		xyz_i.x = eph_today.pos.x;
		xyz_i.y = eph_today.pos.y;
		xyz_i.z = eph_today.pos.z;
		xyzj2b( xyz_i, &xyz_o );
		pos1.x = xyz_o.x;
		pos1.y = xyz_o.y;
		pos1.z = xyz_o.z;
	}
	ra_decl( pos1, &ra, &decl );
	output.Eq1950.ra   = ra;
	output.Eq1950.decl = decl;

	if ( ctrl_sw.B1950_OUT ) {
		chars += eqpos_out( output.Eq1950.ra, output.Eq1950.decl, outfp );
	}
	if ( ctrl_sw.J2000_OUT ) {
		if ( ctrl_sw.orb_el_system == B1950_IN ) {
			Xyz	xyz_i,	xyz_o;

			xyz_i.x = eph_today.pos.x;
			xyz_i.y = eph_today.pos.y;
			xyz_i.z = eph_today.pos.z;
			xyzb2j( xyz_i, &xyz_o );
			pos1.x = xyz_o.x;
			pos1.y = xyz_o.y;
			pos1.z = xyz_o.z;
		} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
			pos1 = eph_today.pos;
		}
		ra_decl( pos1, &ra, &decl );
		output.Eq2000.ra   = ra;
		output.Eq2000.decl = decl;
		chars += eqpos_out( output.Eq2000.ra, output.Eq2000.decl, outfp );
	}
	if ( ctrl_sw.DATE_OUT ) {
		if ( ctrl_sw.orb_el_system == B1950_IN ) {
			prec_b( JD1950, jde, eph_today.pos, &pos1 );
		} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
			prec_b( JD2000, jde, eph_today.pos, &pos1 );
		}
		ra_decl( pos1, &ra, &decl );
		output.EqDate.ra   = ra;
		output.EqDate.decl = decl;
		chars += eqpos_out( output.EqDate.ra, output.EqDate.decl, outfp );
	}
	if ( ctrl_sw.metcalf_sw || ctrl_sw.height_sw ) {
		if ( ctrl_sw.orb_el_system == B1950_IN ) {
			prec_b( JD1950, jde, eph_today.pos, &pos1 );
		} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
			prec_b( JD2000, jde, eph_today.pos, &pos1 );
		}
		ra_decl( pos1, &wk_ra, &wk_decl );
		output.EqDate.ra   = wk_ra;
		output.EqDate.decl = wk_decl;
	}
	if ( ctrl_sw.eph_out_mode == EPH_O_NORMAL ) {		/* NORMAL format */
		if ( ctrl_sw.IAUC_sw != REQUEST
		  || ctrl_sw.IAUC_sw && n % 2 == 0 ) {
			output.delta = eph_today.delta;
			output.r_obj = eph_today.r_obj;
			if ( ctrl_sw.delta_sw ) {
				chars += eph_printf( "%7.3lf", output.delta );
			}
			if ( ctrl_sw.radius_sw ) {
				chars += eph_printf( "%7.3lf", output.r_obj );
			}
			if ( ctrl_sw.d_mot_sw || ctrl_sw.mot_pa_sw ) {
				ra_decl( eph_tomorrow.pos, &ra1, &decl1 );
				if (ctrl_sw.J2000_OUT && ctrl_sw.orb_el_system==B1950_IN) {
					prec_b( JD1950, JD2000, eph_tomorrow.pos, &pos1 );
					ra_decl( pos1, &ra1, &decl1 );
				} else if (ctrl_sw.B1950_OUT && ctrl_sw.orb_el_system==J2000_IN) {
					prec_b( JD2000, JD1950, eph_tomorrow.pos, &pos1 );
					ra_decl( pos1, &ra1, &decl1 );
				}
				if ( ctrl_sw.d_mot_sw ) {
					output.decl_d_mot = decl1 - decl;
					output.ra_d_mot = in_pi( ra1 - ra );

					output.decl_d_mot /= current.step;
					output.ra_d_mot   /= current.step;

					chars += eph_printf( " " );
					chars += eph_printf( "%+6.2lf %+6.1lf",
								RAD2DEG( output.ra_d_mot   ) / 15.0 * 60.0,
								RAD2DEG( output.decl_d_mot ) * 60.0  );
				}
			}
			if ( ctrl_sw.var_T_sw ) {
				if ( ctrl_sw.ent_obj == COMET_IN ) {
					var_T( jde, orb_el_buf, eph_c, ra, decl,
						   current.longitude, current.latitude, current.height,
						   &T_ra, &T_decl );
				} else {
					var_M( jde, orb_el_buf, eph_c, ra, decl,
						   current.longitude, current.latitude, current.height,
						   &T_ra, &T_decl );
				}
				chars += eph_printf( " " );
				chars += eph_printf( "%+6.2lf %+6.1lf",
								RAD2DEG( T_ra   ) / 15.0 * 60.0,
								RAD2DEG( T_decl ) * 60.0   );
			}
			output.pos_angle = position_angle( eph_today.lbr_r, output.r_obj,
											 output.delta );
			if ( ctrl_sw.mag1_sw && ctrl_sw.ent_obj == COMET_IN ) {
				output.mag1 = comet_mag1( orb_el_buf.h1, orb_el_buf.k1,
											 output.delta, output.r_obj );
				chars += eph_printf( "%6.1lf", output.mag1 );
			}
			if ( ctrl_sw.mag2_sw && ctrl_sw.ent_obj == COMET_IN ) {
				output.mag2 = comet_mag2( orb_el_buf.h2, orb_el_buf.k2,
							 output.delta, output.r_obj, output.pos_angle );
				chars += eph_printf( "%6.1lf", output.mag2 );
			}
			if ( ctrl_sw.vmag_sw && ctrl_sw.ent_obj == ASTEROID_IN ) {
				if ( orb_el_buf.g == 0.0 ) {
					output.mag2 = mp_v_mag( orb_el_buf.h2, output.delta,
											 output.r_obj, output.pos_angle );
				} else {
					output.mag2 = mp_p_mag( orb_el_buf.h2, orb_el_buf.g,
							 output.delta, output.r_obj, output.pos_angle );
				}
				chars += eph_printf( "%6.1lf", output.mag2 );
			}
			if ( ctrl_sw.mot_pa_sw ) {
				double	mott,	paa;

				lenth_pa( ra, decl, ra1, decl1,
									   &mott, &paa );
				paa = in_pix2( paa );
				chars += eph_printf( "%6.1lf/%3.0lf", 
								 RAD2DEG( mott ) * 60.0 / current.step,
								 RAD2DEG( paa ) );
			}
			if ( ctrl_sw.elong_sw ) {
				output.elong = elongation( eph_today.lbr_r, output.r_obj,
										 output.delta );
				chars += eph_printf( "%7.1lf", RAD2DEG( output.elong ) );
			}
			if ( ctrl_sw.phase_sw ) {
				chars += eph_printf( "%7.1lf", RAD2DEG( output.pos_angle ) );
			}
			if ( ctrl_sw.tail_sw ) {
				tail( pos1, xyz, decl, output.delta, orb_el_buf.tail,
					 output.pos_angle, &output.phase_angle, &output.tail_l );
				output.phase_angle = in_pix2( output.phase_angle );
				chars += eph_printf( "%5.0lf", RAD2DEG(output.phase_angle) );
				if ( RAD2DEG(output.tail_l) < 10.0 ) {
					chars += eph_printf( "/%4.2lf", RAD2DEG( output.tail_l ) );
				} else if ( RAD2DEG(output.tail_l) < 100.0 ) {
					chars += eph_printf( "/%4.1lf", RAD2DEG( output.tail_l ) );
				} else {
					chars += eph_printf( "/%4.0lf", RAD2DEG( output.tail_l ) );
				}
			}
			if ( ctrl_sw.height_sw ) {
				equ2hor( jde, current.longitude, current.latitude,
						 output.EqDate.ra, output.EqDate.decl,
						&output.azimuth, &output.height );
				chars += eph_printf( "%6.1lf/%5.1lf", RAD2DEG( output.height  ),												RAD2DEG( output.azimuth )   );
			}
			if ( ctrl_sw.moon_sw ) {
				moon(jde, JD1950, &moon_ra, &moon_decl, &sin_pai);
				lenth_pa( ra, decl, moon_ra, moon_decl, &moon_elong,
							&moon_pa );
				output.moon_age = moon_age( jde );
				chars += eph_printf( "  %3.0lf%4.0lf",
									RAD2DEG(moon_elong), output.moon_age );
			}
			if ( ctrl_sw.moon_light_sw ) {
				if ( !ctrl_sw.moon_sw ) {
					moon(jde, JD1950, &moon_ra, &moon_decl, &sin_pai);
				}
				equ2hor( jde, current.longitude, current.latitude,
						 moon_ra, moon_decl,
						&output.azimuth, &output.height );
				chars += eph_printf( "%6.1lf/%5.1lf", RAD2DEG( output.height  ),												RAD2DEG( output.azimuth )   );
			}
			if ( ctrl_sw.metcalf_sw ) {
				char	met_dir;

				metcalf( jde, orb_el_buf, eph_c,
						 output.EqDate.ra, output.EqDate.decl,
						 current.longitude, current.latitude, current.height,
						 &mot, &pa );
				if ( pa > 0.0 ) {
					pa = PI / 2.0 - pa;
					met_dir = 'E';
				} else {
					pa += ( PI / 2.0 );
					met_dir = 'W';
				}
				metcalf_mot = current.metcalf_j * current.metcalf_p * 60.0
								/ RAD2DEG( mot );
				if ( metcalf_mot < 10.0 ) {
					chars += eph_printf( "%7.3lf/%+5.1lf%c",
									metcalf_mot,
									RAD2DEG( pa ),
									met_dir   );
				} else if ( metcalf_mot < 100.0 ) {
					chars += eph_printf( "%7.2lf/%+5.1lf%c",
									metcalf_mot,
									RAD2DEG( pa ),
									met_dir   );
				} else {
					chars += eph_printf( "%7.1lf/%+5.1lf%c",
									metcalf_mot,
									RAD2DEG( pa ),
									met_dir   );
				}
			}
			if ( ctrl_sw.sun2obj_sw ) {
				pos0.x=eph_today.pos.x - xyz.x;
				pos0.y=eph_today.pos.y - xyz.y;
				pos0.z=eph_today.pos.z - xyz.z;
				prec_b( JD1950, jde, pos0, &pos_date );
				sun2obj( pos_date, jde, &Bo, &Lo );
				chars += eph_printf( "%7.1lf/%+5.1lf", Lo, Bo );
			}
			if ( ctrl_sw.e2t_sw ) {
				theta = earth2tail( eph_c, pos1, output.delta );
				chars += eph_printf( "%+6.1lf", RAD2DEG( theta ) );
			}
		}
	}
	eph_printf( "\n" );

	return ( --chars / EPH_cols + 1 );
}

/*
 *	Output routine for Identification
 *
 *		by S.Yamane
 *		by K.Osada
 */
int
id_header( jd, fpo )
double	jd;
FILE	*fpo;
{
	register int	chars = 0,	id_lines = 0;
	Time	tm_p;
	double	day;

	out_fp2 = fpo;

	if ( ctrl_sw.id_out_mode == ID_O_HA ) {				/* h/A format */
		chars += eph_printf( "-------------------------------------------" );
		chars += eph_printf( "------------------------------------\n" ) - 1;
		chars += eph_printf( "Identified Object Name                  " );
		if ( ctrl_sw.orb_el_system == B1950_IN ) {
			chars += eph_printf( "R.A.  (1950)  Decl.        h/A      " );
		} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
			chars += eph_printf( "R.A.  (2000)  Decl.        h/A      " );
		}
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			chars += eph_printf( "m1\n" ) - 1;
		} else {
			chars += eph_printf( "V\n" ) - 1;
		}
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "-------------------------------------------" );
		chars += eph_printf( "------------------------------------\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "                                       " );
		chars += eph_printf( "h  m  s      o  '  \"      o     o\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;
	} else if ( ctrl_sw.id_out_mode == ID_O_OC ) {		/* O-C format */
		chars += eph_printf( "-------------------------------------------" );
		chars += eph_printf( "------------------------------------\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "                                        " );
		chars += eph_printf( "       O - C\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "Identified Object Name                " );
		chars += eph_printf( "------------------------\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "                                        " );
		if ( ctrl_sw.orb_el_system == B1950_IN ) {
			chars += eph_printf( "R.A.  (1950)   Decl.    Distance    " );
		} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
			chars += eph_printf( "R.A.  (2000)   Decl.    Distance    " );
		}
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			chars += eph_printf( "m1\n" ) - 1;
		} else {
			chars += eph_printf( "V\n" ) - 1;
		}
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "-------------------------------------------" );
		chars += eph_printf( "------------------------------------\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;

		chars = 0;
		chars += eph_printf( "                                        " );
		chars += eph_printf( "h  m  s      o  '  \"     o  '  \"\n" ) - 1;
		id_lines += ( chars - 1 ) / EPH_cols + 1;
	}

	return ( id_lines );
}

/*
 *	id_out ---
 *		Identification: h/A format
 */
int
id_out( fpo, roman, number, name, ra, decl, azimuth, altitude, mag )
FILE	*fpo;
char	*roman,	*number,	*name;
double	ra, decl, azimuth,	altitude,	mag;
{
	register int	i = 0;
	int			sg_ra,	sg_dc;
	int			ra_h, ra_m, dc_d, dc_m;
	double		ra_s, dc_s;

	out_fp2 = fpo;

	ra = in_pix2( ra );
	sg_ra = ra2hms(  ra,   &ra_h, &ra_m, &ra_s );
	sg_dc = rad2dms( decl, &dc_d, &dc_m, &dc_s );

	if ( ctrl_sw.ent_obj == COMET_IN ) {
		if (isspace(number[4])) {
			i = eph_printf(  "%-11.11s",    roman );
		} else {
			i = eph_printf(  "%-6.6s     ", number );
		}
	} else {
		if (isdigit(name[0]) && isspace(name[6])) {
			i = eph_printf(  "%-6.6s     ", name );
		} else {
			i = eph_printf(  "%-6.6s     ", number );
		}
	}

	i += eph_printf( "%-26.26s",			name );
	i += eph_printf( "%02d %02d %05.2lf ", ra_h, ra_m, ra_s );
	i += eph_printf( "%c",				  sg_dc>0 ? '+' : '-' );
	i += eph_printf( "%02d %02d %04.1lf  ", dc_d, dc_m, dc_s );
	i += eph_printf( "%5.1lf/%5.1lf  ", RAD2DEG(altitude), RAD2DEG(azimuth) );
	i += eph_printf( "%4.1lf\n",		   mag ) - 1;

	return ( --i / EPH_cols + 1 );
}

/*
 *	oc_out ---
 *		Identification: O-C format
 */
int
oc_out( fpo, roman, number, name, oc_ra, oc_decl, distance, mag )
FILE	*fpo;
char	*roman,	*number,	*name;
double	oc_ra,	oc_decl,	distance,	mag;
{
	register int	i = 0;
	int			sg_ra,	sg_dc,	sg_oc_ra,	sg_oc_dc, sg_dis_dc;
	int			oc_ra_h, oc_ra_m, oc_dc_d, oc_dc_m;
	int			dis_dc_d, dis_dc_m;
	double		oc_ra_s, oc_dc_s, dis_dc_s;

	out_fp2 = fpo;

	sg_oc_ra = ra2hms( oc_ra,   &oc_ra_h, &oc_ra_m, &oc_ra_s );
	sg_oc_dc = rad2dms( oc_decl, &oc_dc_d, &oc_dc_m, &oc_dc_s );
	sg_dis_dc = rad2dms( distance, &dis_dc_d, &dis_dc_m, &dis_dc_s );

	if ( ctrl_sw.ent_obj == COMET_IN ) {
		if (isspace(number[4])) {
			i = eph_printf(  "%-11.11s",    roman );
		} else {
			i = eph_printf(  "%-6.6s     ", number );
		}
	} else {
		if (isdigit(name[0]) && isspace(name[6])) {
			i = eph_printf(  "%-6.6s     ", name );
		} else {
			i = eph_printf(  "%-6.6s     ", number );
		}
	}

	i += eph_printf( "%-26.26s",			name );
	i += eph_printf( "%c",				  sg_oc_ra>0 ? '+' : '-' );
	i += eph_printf( "%02d %02d %05.2lf ", oc_ra_h, oc_ra_m, oc_ra_s );
	i += eph_printf( "%c",				  sg_oc_dc>0 ? '+' : '-' );
	i += eph_printf( "%02d %02d %04.1lf ", oc_dc_d, oc_dc_m, oc_dc_s );
	i += eph_printf( "%3d %02d %04.1lf  ", dis_dc_d, dis_dc_m, dis_dc_s );
	i += eph_printf( "%4.1lf\n",		   mag ) - 1;

	return ( --i / EPH_cols + 1 );
}

int
id_trailer( fpo )
FILE	*fpo;
{
	register int	chars = 0;

	out_fp2 = fpo;

	if ( ctrl_sw.id_out_mode == ID_O_HA
	  || ctrl_sw.id_out_mode == ID_O_OC ) {
		chars += eph_printf( "-------------------------------------------" );
		chars += eph_printf( "------------------------------------\n" ) - 1;
	}

	return ( chars / EPH_cols + 1 );
}

/* **************  end of output.c  ************** */
