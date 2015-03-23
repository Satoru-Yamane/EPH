/*
 *	eph_calc.c
 *
 *	(C)opyright 1988, 1989, 1990 All right reserved
 *						by K.Osada
 *						by S.Yamane
 *
 *  Dec. 03, 1990		by K.Osada
 *
 *	$Header: EPH_CALC.Cv  1.7  90/12/14 08:58:28  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>

#include	"astrolib.h"
#include	"eph.h"

/*
 *	vector_c ---
 *		calculation of vector constant
 *
 *	Program : Coded by S.Yamane
 *  Program : Coded by K.Osada
 *
 *	Jan. 28, 1988
 *	Feb. 11, 1988
 *  Nov. 25, 1990        by K.Osada
 *
 */
void
vector_c( element, eph_c )
Orbit_element	element;			/*  input.  */
Eph_const	*eph_c;				/*  output  */
{
	double	w1,	w2,	w3,	w4,	w5,	w6,	w7,	w8;
	double	sin_peri,	sin_node,	sin_incl;
	double	cos_peri,	cos_node,	cos_incl;
	double	ecl_sin,	ecl_cos;

	sin_peri = sin( element.peri );	cos_peri = cos( element.peri );
	sin_node = sin( element.node );	cos_node = cos( element.node );
	sin_incl = sin( element.incl );	cos_incl = cos( element.incl );

	w1 = cos_peri * cos_node;		w2 = cos_peri * sin_node;
	w3 = sin_peri * cos_node;		w4 = sin_peri * sin_node;
	w5 = sin_peri * sin_incl;		w6 = cos_peri * sin_incl;
	w7 = w3 * cos_incl + w2;		w8 = w1 * cos_incl - w4;

	eph_c->Px = w1 - w4 * cos_incl;
	eph_c->Qx = -( w3 + w2 * cos_incl );

	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		ecl_sin = ECL_SIN1950;
		ecl_cos = ECL_COS1950;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		ecl_sin = ECL_SIN2000;
		ecl_cos = ECL_COS2000;
	}

	eph_c->Py = w7 * ecl_cos - w5 * ecl_sin;
	eph_c->Qy = w8 * ecl_cos - w6 * ecl_sin;
	eph_c->Pz = w7 * ecl_sin + w5 * ecl_cos;
	eph_c->Qz = w8 * ecl_sin + w6 * ecl_cos;
}


/*
 *	parallax ---
 *		correct parallax
 *
 *  Program : Coded by K.Osada
 *  Dec. 05, 1990        by K.Osada
 *
 */
void
parallax( longi, lati, high, jd, d_xyz )
double	longi,	lati,	high,	jd;		/* input  */
Xyz	*d_xyz;								/* output */
{
	double	s,	c,	dxy, dz, theta;
	double	f1, f2 , f3;
	double  cos_2, cos_4, cos_6, sin_solar_parallax;

	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		sin_solar_parallax = sin(DEG2RAD(8.794/60.0/60.0));
		f1 = 1.0 / 298.25;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		sin_solar_parallax = sin(DEG2RAD(8.794148/60.0/60.0));
		f1 = 1.0 / 298.257;
	}

	f2 = f1 * f1;
	f3 = f2 * f1;

	cos_2 = cos(2.0*lati); cos_4 = cos(4.0*lati); cos_6 = cos(6.0*lati);

	s = 1.0 -3.0/2.0*f1 + 5.0/16.0*f2 + 3.0/32.0*f3
	  - (1.0/2.0*f1 - 1.0/2.0*f2 - 5.0/64.0*f3)*cos_2
	  + (3.0/16.0*f2 - 3.0/32.0*f3)*cos_4
	  - 5.0/64.0*f3*cos_6;

	c = 1.0 +1.0/2.0*f1 + 5.0/16.0*f2 + 7.0/32.0*f3
	  - (1.0/2.0*f1 + 1.0/2.0*f2 + 27.0/64.0*f3)*cos_2
	  + (3.0/16.0*f2 + 9.0/32.0*f3)*cos_4
	  - 5.0/64.0*f3*cos_6;

	dxy = -sin_solar_parallax*(c+1.568e-7*high)*cos(lati);
	dz  = -sin_solar_parallax*(s+1.568e-7*high)*sin(lati);

	theta = lst_date(jd) + longi;

	d_xyz->x = dxy * cos(theta);
	d_xyz->y = dxy * sin(theta);
	d_xyz->z = dz;
}


/*
 *	prec_b ---
 *		precession ( B1950 & J2000 & Date )
 *
 *	Apr. 23, 1988        by K.Osada
 *  Nov. 25, 1990        by K.Osada
 *  Dec. 03, 1990        by K.Osada
 */
void
prec_b( jd0, jd, pos1, pos2 )
double		jd0,	jd;		/* input   */
Xyz	pos1;				/* input   */
Xyz	*pos2;				/* output  */
{
	double	t0,	tt0, t, tt, ttt;
	double	wk1,	wk2,	wk3;
	double	sin_wk1,	sin_wk2,	sin_wk3,
			cos_wk1,	cos_wk2,	cos_wk3;

	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		t0 = (jd0 - JD1950) / 36524.2194;
		t = (jd - jd0) / 36524.2194;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		t0 = (jd0 - JD2000) / 36525.0;
		t = (jd - jd0) / 36525.0;
		tt0 = t0 * t0;
	}

	tt = t * t;
	ttt = tt * t;

	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		wk1 = ( 2304.250 + 1.396*t0 )*t + 0.302*tt + 0.018*ttt;
		wk2 = wk1 + 0.791*tt + 0.001*ttt;
		wk3 = ( 2004.682 - 0.853*t0 )*t - 0.426*tt - 0.042*ttt;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		wk1 = ( 2306.2181 + 1.39656*t0 - 0.000139*tt0 )*t
			 + ( 0.30188 - 0.000344*t0 )*tt + 0.017998*ttt;
		wk2 = wk1 + ( 0.79280 + 0.000410*t0 )*tt + 0.000205*ttt;
		wk3 = ( 2004.3109 - 0.85330*t0 - 0.000217*tt0 )*t
			 - ( 0.42665 + 0.000217*t0 )*tt - 0.041833*ttt;
	}

	sin_wk1 = sin(DEG2RAD( wk1 / 60.0 / 60.0 ));
	sin_wk2 = sin(DEG2RAD( wk2 / 60.0 / 60.0 ));
	sin_wk3 = sin(DEG2RAD( wk3 / 60.0 / 60.0 ));
	cos_wk1 = cos(DEG2RAD( wk1 / 60.0 / 60.0 ));
	cos_wk2 = cos(DEG2RAD( wk2 / 60.0 / 60.0 ));
	cos_wk3 = cos(DEG2RAD( wk3 / 60.0 / 60.0 ));

	pos2->x = ( cos_wk1*cos_wk2*cos_wk3 - sin_wk1*sin_wk2 ) * pos1.x
			+ ( -sin_wk1*cos_wk2*cos_wk3 - cos_wk1*sin_wk2 ) * pos1.y
			+ ( -cos_wk2*sin_wk3 ) * pos1.z;
	pos2->y = ( cos_wk1*sin_wk2*cos_wk3 + sin_wk1*cos_wk2 ) * pos1.x
			+ ( -sin_wk1*sin_wk2*cos_wk3 + cos_wk1*cos_wk2 ) * pos1.y
			+ ( -sin_wk2*sin_wk3 ) * pos1.z;
	pos2->z = cos_wk1*sin_wk3*pos1.x
			+ ( -sin_wk1*sin_wk3 )*pos1.y
			+ cos_wk3*pos1.z;
}


/*
 *	xyzb2j ---
 *
 * Coded by K.Osada
 *  Dec. 05, 1990        by K.Osada
 *
 */
Xyz
xyzb2j( xyz1950, xyz2000 )
Xyz	xyz1950;		/*  Input  */
Xyz	*xyz2000;		/*  Output */
{
	double	q, wk_x, wk_y, wk_z;

	q  = -1.62557*xyz1950.x - 0.31919*xyz1950.y - 0.13843*xyz1950.z;

	wk_x = xyz1950.x + (1.62557+q*xyz1950.x)*1.0e-6;
	wk_y = xyz1950.y + (0.31919+q*xyz1950.y)*1.0e-6;
	wk_z = xyz1950.z + (0.13843+q*xyz1950.z)*1.0e-6;

	xyz2000->x = 0.9999256782*wk_x - 0.0111820610*wk_y - 0.0048579477*wk_z;
	xyz2000->y = 0.0111820609*wk_x + 0.9999374784*wk_y - 0.0000271765*wk_z;
	xyz2000->z = 0.0048579479*wk_x - 0.0000271474*wk_y + 0.9999881997*wk_z;

	return ( *xyz2000 );
}


/*
 *	xyzj2b ---
 *
 * Coded by K.Osada
 *  Dec. 05, 1990        by K.Osada
 *
 */
Xyz
xyzj2b( xyz2000, xyz1950 )
Xyz	xyz2000;		/*  Input  */
Xyz	*xyz1950;		/*  Output */
{
	double	q, r0, r1;
	Xyz		xyz1, xyz2;

	xyz1.x = 0.9999256795*xyz2000.x + 0.0111814828*xyz2000.y + 0.0048590039*xyz2000.z;
	xyz1.y = -0.0111814828*xyz2000.x + 0.9999374849*xyz2000.y - 0.0000271771*xyz2000.z;
	xyz1.z = -0.0048590040*xyz2000.x - 0.0000271557*xyz2000.y + 0.9999881946*xyz2000.z;

	r1 = sqrt( xyz1.x*xyz1.x + xyz1.y*xyz1.y + xyz1.z*xyz1.z );

	do {
		xyz2.x = xyz1.x / r1;
		xyz2.y = xyz1.y / r1;
		xyz2.z = xyz1.z / r1;

		q  = -1.62557*xyz2.x - 0.31919*xyz2.y - 0.13843*xyz2.z;

		xyz1.x = xyz2.x + (-1.62557-q*xyz2.x)*1.0e-6;
		xyz1.y = xyz2.y + (-0.31919-q*xyz2.y)*1.0e-6;
		xyz1.z = xyz2.z + (-0.13843-q*xyz2.z)*1.0e-6;

		r0 = r1;
		r1 = sqrt( xyz1.x*xyz1.x + xyz1.y*xyz1.y + xyz1.z*xyz1.z );

	} while( fabs(r0-r1)>1.0e-5 );

	xyz1950->x = xyz1.x / r1;
	xyz1950->y = xyz1.y / r1;
	xyz1950->z = xyz1.z / r1;

	return ( *xyz1950 );
}


/*
 *	obj_pos ---
 *		geocentric equatorial rectanguler coordinates
 *
 *  Program : Coded by K.Osada
 *  Nov. 25, 1990
 *  Dec. 05, 1990        by K.Osada
 */
void
obj_pos( eph_c, r_cosv, r_sinv, xyz, pos )
Eph_const	eph_c;				/* input */
Xyz		xyz;					/* input */
double		r_cosv,	r_sinv;		/* input */
Xyz		*pos;					/* output */
{
	pos->x =  r_cosv * eph_c.Px + r_sinv * eph_c.Qx + xyz.x;
	pos->y =  r_cosv * eph_c.Py + r_sinv * eph_c.Qy + xyz.y;
	pos->z =  r_cosv * eph_c.Pz + r_sinv * eph_c.Qz + xyz.z;
}


/*
 *	locate ---
 *		equatorial coordinates
 *
 *  Program : Coded by K.Osada
 *	Feb. 11, 1988
 *  Nov. 25, 1990        by K.Osada
 */
void
locate( pos, ra, decl, delta )
Xyz	pos;				/*  input. */
double	*ra,	*decl,	*delta;		/*  output */
{
	*ra	= atan2( pos.y, pos.x );
	*decl  = atan2( pos.z, hypot( pos.x, pos.y ) );
	*delta = sqrt(  pos.x * pos.x + pos.y * pos.y + pos.z * pos.z );
}

/*
 *	eph_calc ---
 *  Program : Coded by K.Osada
 *      Nov. 25, 1990
 *
 */
void
eph_calc( jde, element, eph_c, xyz1950, 
			pos_out, ra_out, decl_out, delta_out, r_obj_out )
double		jde;			/* input  */
Orbit_element	element;
Eph_const	eph_c;
Xyz		xyz1950;
Xyz		*pos_out;		/* output */
double		*ra_out, *decl_out, *delta_out, *r_obj_out;
{
	double	delta0,		obj_jde,	interval;
	double	r_cosv,		r_sinv;
	double	ra,		decl,		delta,		r_obj;
	Xyz		pos;

	delta = 1.5;
	do {
		delta0 = delta;
		obj_jde = jde - LIGHT_TIME * (delta);
		interval = obj_jde - element.time_pass;
		if ( element.ecc>0.95 ) {
			n_parabola( element, interval, &r_cosv, &r_sinv, &r_obj );
		} else {
			elipse( element, interval, &r_cosv, &r_sinv, &r_obj );
		}
		obj_pos( eph_c, r_cosv, r_sinv, xyz1950, &pos );
		locate( pos, &ra, &decl, &delta );
	} while ( fabs(delta-delta0) > 1.0e-9 );

	pos_out->x = pos.x;
	pos_out->y = pos.y;
	pos_out->z = pos.z;
	*ra_out    = ra;
	*decl_out  = decl;
	*delta_out = delta;
	*r_obj_out = r_obj;
}


/*
 *	equ2hor ---
 *		equatorial coordinates system to
 *		horizontal coordinates system
 *
 *  Program : Coded by K.Osada
 *
 *	Apr. 05, 1988
 *  Nov. 25, 1990        by K.Osada
 *  Dec. 05, 1990        by K.Osada
 */
void
equ2hor( jd, longi, lati, ra, decl, azimuth, height )
double	jd,	longi,	lati,	ra,	decl;		/* input. */
double	*azimuth,	*height;			/* output */
{
	double	sin_lati,	cos_lati,	sin_decl,	cos_decl,	sin_h,	cos_h;
	double	h,	wk1,	wk2,	wk3;
	Xyz		pos;

	h = lst_date( jd ) + longi - ra;

	sin_lati = sin(lati);	cos_lati = cos(lati);
	sin_decl = sin(decl);	cos_decl = cos(decl);
	sin_h    = sin(h);   	cos_h    = cos(h);

	pos.x = -cos_lati*sin_decl + sin_lati*cos_decl*cos_h;
	pos.y = cos_decl*sin_h;
	pos.z = sin_lati*sin_decl + cos_lati*cos_decl*cos_h;

	locate ( pos, &wk1, &wk2, &wk3 );
	wk1 += ( wk1 > 0.0 ) ? 0.0 : PIX2;

	*azimuth = wk1;
	*height  = wk2;
}

/*
 *  obl_ecl ---
 *
 *  Program : Coded by K.Osada
 *      Nov. 25, 1990
 */
double
obl_ecl( jd )
double	jd;
{
	double	t,	ecl;

	if ( ctrl_sw.orb_el_system==B1950_IN ) {
		t = ( jd - 2415020.0 ) / 36525.0;
		ecl = DEG2RAD( 23.452294+t*(-0.0130125+t*(-0.00000164+0.000000503*t)) );	} else if ( ctrl_sw.orb_el_system==J2000_IN ) {
		t = ( jd - JD2000 ) / 36525.0;
		ecl = DEG2RAD( 23.439291+t*(-0.0130042+t*(-0.00000163+0.000000503*t)) );	}

	return( ecl );
}

/*
 *	ecl2equ ---
 *
 *  Program : Coded by K.Osada
 *      Nov. 25, 1990
 *		Oct. 22,  1989
 */
void
ecl2equ( jd, ecl_pos, equ_pos )
double	jd;
Xyz	ecl_pos;
Xyz	*equ_pos;
{
	double	ecl,	sin_ecl,	cos_ecl;

	ecl = obl_ecl( jd );

	sin_ecl = sin( ecl );
	cos_ecl = cos( ecl );

	equ_pos->x =  ecl_pos.x;
	equ_pos->y = -ecl_pos.z*sin_ecl + ecl_pos.y*cos_ecl;
	equ_pos->z =  ecl_pos.z*cos_ecl + ecl_pos.y*sin_ecl;
}


/*
 *	equ2ecl ---
 *
 *  Program : Coded by K.Osada
 *		Jan. 19, 1989
 *      Nov. 25, 1990
 */
void
equ2ecl( jd, pos_date, ecl_pos )
double	jd;
Xyz	pos_date;
Xyz	*ecl_pos;
{
	double	ecl,	sin_ecl,	cos_ecl;

	ecl = obl_ecl( jd );

	sin_ecl = sin( ecl );
	cos_ecl = cos( ecl );

	ecl_pos->x =  pos_date.x;
	ecl_pos->y =  pos_date.y * cos_ecl  +  pos_date.z * sin_ecl;
	ecl_pos->z = -pos_date.y * sin_ecl  +  pos_date.z * cos_ecl;
}


/*
 *	sun2obj ---
 *
 *  Program : Coded by K.Osada
 *		Jan. 19, 1989
 *      Nov. 25, 1990
 */
void	sun2obj( pos_date, jd, Bo, Lo )
Xyz	pos_date;
double	jd;
double	*Bo, *Lo;
{
	double	sun_i,	sun_node,	sun_longi;
	double	wk1,	wk2,	wk3,	wk4,	wk5,	wk6;
	Xyz		ecl_pos,	sun_pos;

	equ2ecl( jd, pos_date, &ecl_pos );

	sun_i = DEG2RAD( 7.25 );
	sun_node = DEG2RAD( 75.20 + 50.25/3600.0*(jd-2436934.5)/365.25 );
	wk1 = sin( sun_i );
	wk2 = cos( sun_i );
	wk3 = sin( sun_node );
	wk4 = cos( sun_node );
	wk5 = -ecl_pos.x*wk3 + ecl_pos.y*wk4;

	sun_pos.x = ecl_pos.x*wk4 + ecl_pos.y*wk3;
	sun_pos.y = wk5*wk2 - ecl_pos.z*wk1;
	sun_pos.z = wk5*wk1 + ecl_pos.z*wk2;
	sun_longi = fmod( -360.0/25.38*( jd-2398220.0 ), 360.0 );

	wk6 = sun_longi + RAD2DEG( atan2( sun_pos.y, sun_pos.x ) );
	while ( wk6 < 0.0 ) {
		wk6 += 360.0;
	}
	*Lo = wk6;
	*Bo = RAD2DEG( atan2( sun_pos.z, hypot( sun_pos.x, sun_pos.y ) ) );
}


/*
 *	lenth_pa ---
 *
 *  Program : Coded by K.Osada
 *
 *  	Jan. 05,  1989
 *      Nov. 25, 1990
 *
 */
void
lenth_pa( ra1, decl1, ra2, decl2, lenth, phase )
double	ra1, decl1, ra2, decl2;		/* input  */
double	*lenth, *phase;			/* output */
{
	double	x,	y,	z;
	double	d_ra;
	double	cd1,	cd2,	sd1,	sd2;

	d_ra = ra2 - ra1;
	cd1 = cos( decl1 );
	cd2 = cos( decl2 );
	sd1 = sin( decl1 );
	sd2 = sin( decl2 );
	x = cd1 * sd2  -  sd1 * cd2 * cos( d_ra );
	y = cd2 * sin( d_ra );
	z = sd1 * sd2  +  cd1 * cd2 * cos( d_ra );

	*phase = atan2( y, x );
	*lenth = atan2( hypot( x, y ), z );
}


void
ra_decl( pos, ra, decl )
Xyz	pos;			/* input */
double	*ra,	*decl;
{
	*ra   = atan2( pos.y, pos.x );
	*decl = atan2( pos.z, hypot( pos.x, pos.y ) );
}


/*
 *	Conversion of Co-ordinate for EPH
 */
int
ra2hms( ra, h, m, s )
double	ra;			/* Input : [rad] */
int	*h,	*m;
double	*s;
{
	double	fval,	ival;
	int		sign;		/* return value */

	sign = ( ra<0.0 ) ? -1 : 1;
	fval = RAD2DEG( (double)sign * ra ) / 15.0;
	fval = modf( fval, &ival ) * 60.0;	*h = (int)ival;
	fval = modf( fval, &ival ) * 60.0;	*m = (int)ival;
	*s   = fval;

	return ( sign );
}

double
hms2ra( h, m, s )	/* R.A.[rad] */
int	h,	m;
double	s;
{
	double	retval;

	retval = ( h + ( m + ( s / 60.0 ) ) / 60.0 ) * 15.0;

	return ( DEG2RAD( retval ) );
}

int
rad2dms( rad, d, m, s )
double	rad;			/* Input : [rad] */
int	*d,	*m;
double	*s;
{
	double	fval,	ival;
	int		sign;		/* return value */

	sign = ( rad<0.0 ) ? -1 : 1;
	fval = RAD2DEG( (double)sign * rad );
	fval = modf( fval, &ival ) * 60.0;	*d = (int)ival;
	fval = modf( fval, &ival ) * 60.0;	*m = (int)ival;
	*s   = fval;

	return ( sign );
}

double
dms2rad( d, m, s )	/* Angle[rad] */
int	d,	m;
double	s;
{
	double	sg = 1.0;
	double	retval;

	if ( d<0 ) {
		sg = -1.0;
		d *= -1;
	}
	retval = sg * ( d + ( m + ( s / 60.0 ) ) / 60.0 );

	return ( DEG2RAD( retval ) );
}


double
in_pi( rad_in )
double	rad_in;
{
	double	rad_out;

	while ( rad_in > PI  ) rad_in -= PIX2;
	while ( rad_in < -PI ) rad_in += PIX2;

	rad_out = rad_in;

	return ( rad_out );
}


double
in_pix2( rad_in )
double	rad_in;
{
	double	rad_out;

	while ( rad_in > PIX2 ) rad_in -= PIX2;
	while ( rad_in < 0.0  ) rad_in += PIX2;

	rad_out = rad_in;

	return ( rad_out );
}

/* **************  end of eph_calc.c  ***************** */
