/*
 *		meteor.c : meteor_forecast
 *
 *			by  K.Osada
 *				S.Yamane
 *
 *		Aug. 31,  1989
 *		Nov. 04,  1989
 *		Dec. 21,  1989
 *		Jan. 09,  1991
 *
 *	$Header: METEOR.CV  1.11  91/01/09 09:29:04  yamane  Exp $
 */

#include	<stdio.h>
#include	<math.h>

#include	"astrolib.h"
#include	"menu.h"
#include	"eph.h"

#define		ASCEND		0
#define		DESCEND		1
#define		EARTH_PERI	102.08
#define		EARTH_R		0.99972
#define		EARTH_ECC	0.01672
#define		LIMIT		1.0e-5
#define		BASE2			13

static int		q_type = 0;
static double	year_jd = 2448438.0;
static	double	Lsun;


/*
 *		output_meteor ---
 *
 *		Program : Coded by K.Osada
 *
 *		Aug. 31,  1989
 *		Nov. 04,  1989 
 */
static void	out_of_range( x, y )
int	x,	y;
{
	moveto( x-3, y );		dos_puts("Minimum distance" );
	moveto( x-3, y+1 );		dos_puts("  Over 0.2AU" );
}


static void	appear_date( jd0, node_type, el )
double	jd0;
int	node_type;
Orbit_element	el;
{
	register int	x,	y;
	double			jd,	day;
	Time			tm_p;
	extern double	node2jd();

	if ( node_type == ASCEND ) {
		x = 46;
	} else {
		x = 66;
	}
	y = BASE2 + 1;

	jd = node2jd( jd0, Lsun );
	day = jd2day( jd, &tm_p );
	moveto( x, y );
	dos_printf( "%4u %2u/%5.2lf", tm_p.year, tm_p.mon, day );
	moveto( x-2, y + 1);
	dos_printf( "%+7.1lf", jd - el.time_pass);
}


/*
 *		meteor ---
 *
 *		Program : Coded by K.Osada
 *
 *		Aug. 31,  1989
 *		Nov. 04,  1989 
 *		Jan. 09,  1991
 */
void
meteor( v, orb_el, node_type, meteor_ra, meteor_decl, VG, r_R )
Orbit_element	orb_el;
int				node_type;
double			v, *meteor_ra,	*meteor_decl,	*VG,	*r_R;
{
	double			p, wk, meteor_Lsun;
	double			sinv,	cosv;
	double			xx, yy, zz;
	double			XX, YY, ZZ;
	double			ra, decl, V, R, r;
	Orbit_element	meteor_el;
	Xyz				pos;
	Eph_const		eph_c;

	meteor_el.peri = orb_el.peri;
	meteor_el.node = orb_el.node;
	meteor_el.incl = orb_el.incl;
	meteor_el.ecc = orb_el.ecc;
/*	if ( meteor_el.ecc != 1.0 ) */
	if ( meteor_el.ecc < 1.0 )
		meteor_el.sm_axis = orb_el.sm_axis;
	meteor_el.peri_dis = orb_el.peri_dis;

	vector_c( meteor_el, &eph_c );
	p = meteor_el.peri_dis * ( 1.0 + meteor_el.ecc );
	if ( p == 0.0 )
		return;		/* avoid to devide by Zero */

	wk = GAUSS / sqrt( p );
	sinv = sin( v );
	cosv = cos( v );
	xx = wk * ( -eph_c.Px * sinv + eph_c.Qx * ( meteor_el.ecc + cosv ) );
	yy = wk * ( -eph_c.Py * sinv + eph_c.Qy * ( meteor_el.ecc + cosv ) );
	zz = wk * ( -eph_c.Pz * sinv + eph_c.Qz * ( meteor_el.ecc + cosv ) );
	XX = -0.017205 * sin( Lsun ) + 0.000282;
	YY =  0.015784 * cos( Lsun ) + 0.000055;
	ZZ = YY * 0.433685;
	pos.x = -( xx + XX );
	pos.y = -( yy + YY );
	pos.z = -( zz + ZZ );
	locate( pos, &ra, &decl, &V );
	R = EARTH_R
	 / ( 1.0 + EARTH_ECC * cos( Lsun - RAD2DEG(EARTH_PERI) ) );
	r = p / ( 1.0 + orb_el.ecc * cosv );

	*meteor_ra = in_pix2( ra );
	*meteor_decl = decl;
	*VG = 1731.5 * V;
	*r_R = r - R;
}


/*
 *		meteor_orbit ---
 *
 *		Program : Coded by K.Osada
 *
 *		Aug. 31,  1989
 *		Nov. 04,  1989 
 *		Jan. 09,  1991
 */
static void
meteor_orbit( orb_el, v_type, vv, meteor_z, meteor_el, r_R, node_type )
Orbit_element	orb_el;
int				v_type, *node_type;
double			*vv, *meteor_z,	*r_R;
Orbit_element	*meteor_el;
{
	int	x,	y,	yy;
	double	p,	R0,	R=1.0,	v, u,	L,	r;

	if ( orb_el.peri_dis < 1.2168 && orb_el.peri_dis > 0.9834 ) {
		meteor_el->peri_dis = 0.9834;
	}

	p = orb_el.peri_dis * ( 1.0 + orb_el.ecc );
	if ( p == 0.0 )
		return;				/* avoid to devide by Zero */

	do {
		if ( ( p/R - 1.0 ) / orb_el.ecc > 1.0 ) {
			if (sin(orb_el.peri) > 0) {
				if ( v_type==0 ) {
					*node_type = DESCEND;
				} else {
					*node_type = ASCEND;
				}
			} else {
				if ( v_type==1 ) {
					*node_type = DESCEND;
				} else {
					*node_type = ASCEND;
				}
			}
			x = ( *node_type == ASCEND ) ? 46 : 66;
			y = BASE2;
			yy = 5;

			bold_start();
			out_of_range( x, yy+1 );
			out_of_range( x, y+4 );
			bold_end();
			return;
		}

		v = acos( ( p/R - 1.0 ) / orb_el.ecc );
		if ( v_type )
			v = -v;
		u = in_pix2( orb_el.peri + v );
		L = atan2( sin(u)*cos(orb_el.incl), cos(u) ) + orb_el.node;
		R0 = R;
		R = EARTH_R / ( 1.0 + 0.01672 * cos( L - DEG2RAD(EARTH_PERI) ) );
	} while ( fabs( R - R0 ) > LIMIT );

	Lsun = in_pix2( L + PI );

	if ( orb_el.ecc >= 1.0 ) {
		r = orb_el.peri_dis / cos( v/2.0 ) * cos( v/2.0 );
	} else {
		r = p / ( 1.0 + orb_el.ecc * cos( v ) );
	}
	*r_R = r - R;

	*vv = v;

	meteor_el->incl = fabs( asin(sin(orb_el.incl)*cos(L-orb_el.node)) );
	if ( orb_el.incl > PI/2.0 ) {
		meteor_el->incl = PI - meteor_el->incl;
	}

if (sin(orb_el.peri) > 0) {
		if ( v_type==0 ) {
			meteor_el->peri = in_pix2( PI - v );
			meteor_el->node = in_pix2( L + PI );
			*node_type = DESCEND;
		} else {
			meteor_el->peri = in_pix2( PIX2 - v );
			meteor_el->node = in_pix2( L );
			*node_type = ASCEND;
		}
} else {
		if ( v_type==1 ) {
			meteor_el->peri = in_pix2( PI - v );
			meteor_el->node = in_pix2( L + PI );
			*node_type = DESCEND;
		} else {
			meteor_el->peri = in_pix2( PIX2 - v );
			meteor_el->node = in_pix2( L );
			*node_type = ASCEND;
		}
}

	*meteor_z = R * tan( asin(sin(u)*sin(orb_el.incl)) );

	meteor_el->ecc = orb_el.ecc;
	meteor_el->sm_axis = orb_el.sm_axis;
	meteor_el->peri_dis = orb_el.peri_dis;
}


/*
 *		title ---
 *
 *		Program : Coded by  K.Osada
 *
 *		Aug. 31,  1989
 *		Nov. 04,  1989 
 *		Jan. 09,  1991
 */
void
title( el )
Orbit_element	el;
{
	double	el_day;
	Time	tmm;

	clear_bottom( 0, Y_BIAS );

	moveto( 0, Y_BIAS );
	reverse_start();
	dos_puts( "Orbital elements\n" );
	reverse_end();
	underline_start();
	dos_printf( "%-11.11s (%-7.7s)", el.roman, el.number );
	underline_end();
	EPH_Hline( ' ', 34 );

	reverse_start();
	dos_printf( "(Meteor)\n" );
	dos_printf( "[%30.30s]           ", el.name );
	dos_printf( "Ascending side      Descending side  " );
	reverse_end();

	el_day = jd2day( el.epoch, &tmm );
	p_item( "(Epoch)  :", "   % 4d %s%4.1lf",
						tmm.year, mon2strmon(tmm.mon), el_day );
	el_day = jd2day( el.time_pass, &tmm );
	p_item( " ET\n(T)      :", "   % 4d %s%4.1lf",
						tmm.year, mon2strmon(tmm.mon), el_day );
	p_item( " ET\n(peri.)  :", "%11.3lf\n", RAD2DEG( el.peri ) );
	p_item( "(node)   :", "%11.3lf\n", RAD2DEG( el.node ) );
	p_item( "(incl.)  :", "%11.3lf\n", RAD2DEG( el.incl ) );
	p_item( "(q)      :", "%12.4lf\n", el.peri_dis );
	p_item( "(e)      :", "%12.4lf\n", el.ecc  );

	reverse_start();
	dos_puts( "Appearance Forecast                        Ascending side      Descending side  " );
	reverse_end();

	bold_start();
	EPH_Hline( ' ', 16);	dos_puts( "L        (deg)  : \n" );	/* ¬¿Õ€≤´∑–     */
	EPH_Hline( ' ', 16);	dos_puts( "Date      (UT)  : \n" );	/* Ω–∏Ω«Ø∑Ó∆¸   */
	EPH_Hline( ' ', 16);	dos_puts( "d_T      (day)  : \n" );	/* ∂·∆¸≈¿ƒÃ≤·∆¸§»§Œ∫π */
	EPH_Hline( ' ', 16);	dos_puts( "R.A.     (deg)  : \n" );	/* Ì’ºÕ≈¿¿÷∑–   */
	EPH_Hline( ' ', 16);	dos_puts( "Decl.    (deg)  : \n" );	/* Ì’ºÕ≈¿¿÷∞ﬁ   */
	EPH_Hline( ' ', 16);	dos_puts( "VG      (km/s)  : \n" );	/* ¬–√œ¬Æ≈Ÿ     */
	EPH_Hline( ' ', 16);	dos_puts( "r-R       (AU)  : \n" );	/*  Ï≈∑¬Œ°›√œµÂ∆¸ø¥µ˜Œ•∫π */
	EPH_Hline( ' ', 16);	dos_puts( "z         (AU)  : \n" );	/*  Ï≈∑¬Œ°›≤´∆ª¥÷µ˜Œ• */
	EPH_Hline( ' ', 16);	dos_puts( "d_RA (deg/day)  : \n" );	/* Ì’ºÕ≈¿¿÷∑–∞‹∆∞ */
	EPH_Hline( ' ', 16);	dos_puts( "d_Decl(deg/day) : " );	/* Ì’ºÕ≈¿¿÷∞ﬁ∞‹∆∞ */
	bold_end();
}


/*
 *	output_meteor ---
 */
int
output_meteor( v_type, orb_el )
int				v_type;
Orbit_element	orb_el;
{
	int				x, y, yy, node_type;
	double			v, meteor_z, jd;
	double			meteor_ra, meteor_decl, VG, r_R0, r_R;
	double			meteor_ra1, meteor_decl1, d_ra, d_decl;
	Orbit_element	meteor_el;
	Time			tm_p;
	extern double	node2jd();

	if ( orb_el.peri_dis < 1.2168 ) {
		meteor_orbit(orb_el, v_type, &v, &meteor_z, &meteor_el, &r_R0, &node_type );
	} else {
		bold_start();
		out_of_range( 46, 6 );		out_of_range( 46, 17 );
		out_of_range( 66, 6 );		out_of_range( 66, 17 );
		bold_end();
		return ( 0 );
	}

	x = ( node_type == ASCEND ) ? 46 : 66;
	y = BASE2;
	yy = 5;

	if ( fabs(meteor_z) > 0.2 ) {
		bold_start();
		out_of_range( x, yy+1 );
		out_of_range( x, y+4 );
		bold_end();
		return ( 0 );
	}

	underline_start();

	moveto( x, yy+2 );
	dos_printf( "%7.3lf", RAD2DEG(meteor_el.peri) );
	moveto( x, yy+3 );
	dos_printf( "%7.3lf", RAD2DEG(meteor_el.node) );
	moveto( x, yy+4 );
	dos_printf( "%7.3lf", RAD2DEG(meteor_el.incl) );
	moveto( x, yy+5 );
	dos_printf( "%8.4lf", meteor_el.peri_dis );
	moveto( x, yy+6 );
	dos_printf( "%8.4lf", meteor_el.ecc );

	meteor(v, meteor_el, node_type, &meteor_ra, &meteor_decl, &VG, &r_R);

	moveto( x, y);
	dos_printf( "%6.2lf", RAD2DEG(Lsun));
	appear_date( year_jd, node_type, orb_el );
	moveto( x, y + 3);
	dos_printf( "%5.1lf", RAD2DEG(meteor_ra));
	moveto( x, y + 4);
	dos_printf( "%+5.1lf", RAD2DEG(meteor_decl));
	moveto( x, y + 5);
	dos_printf( "%5.1lf", VG);
	moveto( x, y + 6);
	dos_printf( "%+7.3lf", r_R);
	moveto( x, y + 7);
	dos_printf( "%+7.3lf", meteor_z);

	meteor_el.node += DEG2RAD(0.9856);
	meteor(v, meteor_el, node_type, &meteor_ra1, &meteor_decl1, &VG, &r_R);

	d_ra = meteor_ra1 - meteor_ra;
	d_decl = meteor_decl1 - meteor_decl;
	moveto( x, y + 8);
	dos_printf( "%+6.2lf", RAD2DEG(d_ra));
	moveto( x, y + 9);
	dos_printf( "%+6.2lf", RAD2DEG(d_decl));

	underline_end();

	return ( 1 );		/* exist */
}

/*
 *	global variables in this module
 */
static int		exist_asc = 0,	exist_desc = 0;

/*
 *		change_year()
 *			retry to show appearance date
 *			call by scan_file() when CR pressed.
 */
int
change_year( dmy_fp, dmy1, dmy2, dmy_el, dmy_cm )
FILE			*dmy_fp;
int				dmy1,	dmy2;
Orbit_element	dmy_el;
Comment			dmy_cm;
{
	int		c;
	double	wday;

	wday = year_jd;
	blink_start();
	moveto( 0, BASE2+1 );
	dos_puts( "+ : Forward     Date      (UT)  : \n" );
	dos_puts( "- : Backward    d_T      (day)  : \n" );
	dos_puts( "ESCAPE\n" );
	blink_end();

	underline_start();
	do {
		switch( ( c = getchr() ) ) {
		case '+':
		case ' ':
				wday += 365.2425;
				if ( exist_asc )
					appear_date( wday, ASCEND, dmy_el );
				if ( exist_desc )
					appear_date( wday, DESCEND, dmy_el );
				break;
		case '-':
		case BS:	/* BackSpace */
				wday -= 365.2425;
				if ( exist_asc )
					appear_date( wday, ASCEND, dmy_el );
				if ( exist_desc )
					appear_date( wday, DESCEND, dmy_el );
				break;
		case ESCAPE:
				break;
		default:
				RingBell();
		}
	} while ( c != ESCAPE );
	underline_end();

	return ( 0 );
}


/*
 *		meteor_forecast()
 *			call from scan_file()
 */
int
meteor_forecast( dmy_recno, dmy_recsize, orb_el, dmy_cm )
int				dmy_recno,	dmy_recsize;
Orbit_element	orb_el;
Comment			dmy_cm;
{
	double	jd_now();

	year_jd = jd_now();
	title( orb_el );

	exist_asc  = output_meteor( 0, orb_el );
	exist_desc = output_meteor( 1, orb_el );

	return ( 0 );
}
/* **************  end of meteor.c  ***************** */
