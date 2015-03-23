/*
 *	out_calc.c
 *		EPH output calculation routine
 *
 *	(C)opyright 1990,  all right reserved
 *	   Dec. 08, 1990,	by K.Osada
 *
 */

#include	<math.h>

#include	"astrolib.h"
#include	"eph.h"

double
comet_mag1( h1, k1, delta, r_obj )
double	h1, k1, delta, r_obj;	/* input */
{
	double	m1;				/* output */

	m1 = h1 + 5.0 * log10( delta ) + k1 * log10( r_obj );

	return ( m1 );
}


double
comet_mag2( h2, k2, delta, r_obj, pos_angle )
double	h2, k2, delta, r_obj, pos_angle;	/* input */
{
	double	m2;				/* output */

	m2 = h2	+ 5.0 * log10( delta ) + k2 * log10( r_obj )
		+ 0.03 * RAD2DEG( pos_angle );

	return ( m2 );
}


double
mp_v_mag( h2, delta, r_obj, pos_angle )
double	h2, delta, r_obj, pos_angle;	/* input */
{
	double	mv;				/* output */

	mv = h2 + 5.0 * log10( delta ) + 5.0 * log10( r_obj )
		 + 0.023 * RAD2DEG( pos_angle );

	return ( mv );
}

double
mp_p_mag( h2, g, delta, r_obj, pos_angle )
double	h2, g, delta, r_obj, pos_angle;	/* input */
{
	double	mp;				/* output */
	double	w1,	w2;

	w1 = exp( -3.33 * pow( tan( pos_angle/2.0 ), 0.63 ) );
	w2 = exp( -1.87 * pow( tan( pos_angle/2.0 ), 1.22 ) );
	mp = h2 + 5.0 * log10( delta * r_obj )
		 - 2.5 * log10( ( 1.0 - g ) * w1 + g * w2 );

	return ( mp );
}


double
position_angle( lbr_r, r_obj, delta )
double	lbr_r,	r_obj,	delta;	/* input */
{
	double	pos_angle;				/* output */
	double	wk;

	wk =  ( lbr_r + r_obj - delta ) * ( lbr_r - r_obj + delta )
		 / ( 4.0 * r_obj * delta );
	pos_angle = 2.0 * asin( sqrt( wk ) );

	return ( pos_angle );
}


double
elongation( lbr_r, r_obj, delta )
double	lbr_r,	r_obj,	delta;	/* input */
{
	double	elong;				/* output */
	double	wk;

	wk = ( r_obj + lbr_r - delta ) * ( r_obj - lbr_r + delta )
	   / ( 4.0 * lbr_r * delta );
	elong =  2.0 * asin( sqrt( wk ) );

	return ( elong );
}


void
tail( pos, xyz, decl, delta, tail, pos_angle, phase_angle, tail_l )
double	decl,	delta,	tail,	pos_angle;	/* input */
Xyz			pos;
Xyz			xyz;
double	*phase_angle,	*tail_l;						/* output */
{
	double	wk1,	wk2,	wk3;

	wk1 = pos.y * xyz.x - pos.x * xyz.y;
	wk2 = ( pos.x * xyz.x + pos.y * xyz.y ) * sin( decl )
		  - sqrt( pos.x * pos.x + pos.y * pos.y )
				* xyz.z * cos( decl );
	wk3 = delta / tail + cos( pos_angle );

	*phase_angle = atan2( wk1, wk2 );
	*tail_l = atan2( sin( pos_angle ), wk3 );
}


double
moon_age( jd )
double	jd;		/* input */
{
	double	moon_age;

	moon_age = ( jd + 20.3 - 29.53058885 * floor((jd + 20.3) / 29.53058885 ) );

	return ( moon_age );
}


double
earth2tail( eph_c, pos, delta )
Eph_const	eph_c;		/* input */
Xyz			pos;		/* input */
double		delta;
{
	double	theta;		/* output */
	double	Hx,	Hy, Hz;

	Hx = eph_c.Py*eph_c.Qz - eph_c.Pz*eph_c.Qy;
	Hy = eph_c.Pz*eph_c.Qx - eph_c.Px*eph_c.Qz;
	Hz = eph_c.Px*eph_c.Qy - eph_c.Py*eph_c.Qx;
	theta = PI/2.0 - acos( ( Hx*pos.x + Hy*pos.y + Hz*pos.z ) / delta );

	return ( theta );
}


/*
 *	var_T ---
 *		variation of T
 */
void
var_T( jde, orb_el, eph_c, ra0, decl0, longi, lati, high, T_ra, T_decl )
double	jde, ra0, decl0, longi, lati, high;
Orbit_element	orb_el;
Eph_const	eph_c;
double	*T_ra, *T_decl;
{
	double	ra,	decl,	delta,	r_obj;
	Xyz		xyz1950, d_xyz;
	Lbr		lbr;
	Xyz		pos, pos1;

	orb_el.time_pass += 1.0;

	if ( ctrl_sw.simple_sw ) {
		newcombs(jde, &xyz1950, &lbr);
	} else {
		newcomb1(jde, &xyz1950, &lbr);
	}

	/* correct parallax */
	if ( ctrl_sw.parallax_sw ) {
		parallax( longi, lati, high, jde, &d_xyz );
	} else {
		d_xyz.x = d_xyz.y = d_xyz.z = 0.0;
	}
	xyz1950.x += d_xyz.x;
	xyz1950.y += d_xyz.y;
	xyz1950.z += d_xyz.z;

	eph_calc( jde, orb_el, eph_c, xyz1950, &pos, &ra, &decl, &delta, &r_obj );

	if (ctrl_sw.J2000_OUT && ctrl_sw.orb_el_system==B1950_IN) {
		prec_b( JD1950, JD2000, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}
	if (ctrl_sw.B1950_OUT && ctrl_sw.orb_el_system==J2000_IN) {
		prec_b( JD2000, JD1950, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}

	*T_ra   = in_pi( ra - ra0 );
	*T_decl = decl - decl0;
}


/*
 *	vat_M() :
 *		variation of M
 *
 *	Program : Coded by K.Osada
 *
 *	Apr. 12,  1989
 */
void
var_M( jde, orb_el, eph_c, ra0, decl0, longi, lati, high,
			M_ra, M_decl )
double		jde, ra0, decl0, longi, lati, high;
Orbit_element	orb_el;
Eph_const	eph_c;
double		*M_ra, *M_decl;
{
	double	ra,	decl,	delta,	r_obj;
	Xyz		xyz1950, d_xyz;
	Lbr		lbr;
	Xyz		pos, pos1;

	orb_el.mean_anomaly += DEG2RAD(1.0);
	orb_el.time_pass = orb_el.epoch - orb_el.mean_anomaly 
									/ orb_el.daily_motion;

	if ( ctrl_sw.simple_sw ) {
		newcombs(jde, &xyz1950, &lbr);
	} else {
		newcomb1(jde, &xyz1950, &lbr);
	}

	/* correct parallax */
	if ( ctrl_sw.parallax_sw ) {
		parallax( longi, lati, high, jde, &d_xyz );
	} else {
		d_xyz.x = d_xyz.y = d_xyz.z = 0.0;
	}
	xyz1950.x += d_xyz.x;
	xyz1950.y += d_xyz.y;
	xyz1950.z += d_xyz.z;

	eph_calc( jde, orb_el, eph_c, xyz1950, &pos, &ra, &decl, &delta, &r_obj );

	if (ctrl_sw.J2000_OUT && ctrl_sw.orb_el_system==B1950_IN) {
		prec_b( JD1950, JD2000, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}
	if (ctrl_sw.B1950_OUT && ctrl_sw.orb_el_system==J2000_IN) {
		prec_b( JD2000, JD1950, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}

	*M_ra   = in_pi( ra - ra0 );
	*M_decl = decl - decl0;
}

/*
 *	metcalf ---
 *		calculate Const. of Metcalf method
 */
void
metcalf( jde0, orb_el, eph_c, ra0, decl0, longi, lati, high,
			metcalf_mot, metcalf_pa )
double			jde0, ra0, decl0, longi, lati, high;
Orbit_element	orb_el;
Eph_const		eph_c;
double			*metcalf_mot, *metcalf_pa;
{
	double	jde,	 ra,	decl,	delta,	r_obj,	mot,	pa;
	Xyz		xyz1950,	d_xyz;
	Lbr		lbr;
	Xyz		pos,	pos1;

	jde = jde0 + 1.0 / 24.0;		/* add to 1 hr. */

	if ( ctrl_sw.simple_sw ) {
		newcombs(jde, &xyz1950, &lbr);
	} else {
		newcomb1(jde, &xyz1950, &lbr);
	}

	/*
	 *	correct parallax
	 */
	if ( ctrl_sw.parallax_sw ) {
		parallax(longi, lati, high, jde, &d_xyz);
	} else {
		d_xyz.x = d_xyz.y = d_xyz.z = 0.0;
	}
	xyz1950.x += d_xyz.x;
	xyz1950.y += d_xyz.y;
	xyz1950.z += d_xyz.z;

	eph_calc(jde, orb_el, eph_c, xyz1950, &pos, &ra, &decl, &delta, &r_obj);

	if (ctrl_sw.orb_el_system==B1950_IN) {
		prec_b( JD1950, jde, pos, &pos1 );		/* B1950 -> date */
		ra_decl( pos1, &ra, &decl );			/* equinox: date */
	}
	if (ctrl_sw.orb_el_system==J2000_IN) {
		prec_b( JD2000, jde, pos, &pos1 );		/* J2000 -> date */
		ra_decl( pos1, &ra, &decl );			/* equinox: date */
	}

	lenth_pa(ra0, decl0, ra, decl, &mot, &pa);

	*metcalf_mot = mot / 60.0;
	*metcalf_pa = pa;
}

/*
 *	daily_m ---
 *		daily motion ( exactly 1.0 day )
 */
void
daily_m( jde0, orb_el, eph_c, ra0, decl0, longi, lati, high,
					d_ra, d_decl )
double			jde0,	ra0,	decl0,	longi,	lati,	high;
Orbit_element	orb_el;
Eph_const		eph_c;
double			*d_ra,	*d_decl;
{
	double	jde,	ra,	decl,	delta,	r_obj,	mot,	pa;
	Xyz		xyz1950,	d_xyz;
	Lbr		lbr;
	Xyz		pos,	pos1;

	jde = jde0 + 1.0;		/* add to 1 day. */

	newcomb1(jde, &xyz1950, &lbr);

	/*
	 *	correct parallax
	 */
	parallax(longi, lati, high, jde, &d_xyz);
	xyz1950.x += d_xyz.x;
	xyz1950.y += d_xyz.y;
	xyz1950.z += d_xyz.z;

	eph_calc(jde, orb_el, eph_c, xyz1950, &pos, &ra, &decl, &delta, &r_obj);

	if (ctrl_sw.J2000_OUT && ctrl_sw.orb_el_system==B1950_IN) {
		prec_b( JD1950, JD2000, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}
	if (ctrl_sw.B1950_OUT && ctrl_sw.orb_el_system==J2000_IN) {
		prec_b( JD2000, JD1950, pos, &pos1 );
		ra_decl( pos1, &ra, &decl );
	}

	*d_ra   = in_pi( ra - ra0 );
	*d_decl = decl - decl0;
}

/* **************  end of out_calc.c  ************** */
