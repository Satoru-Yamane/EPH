/*
 *	orbit.c
 *		Elipse / Near parabolic / Parabolic orbital
 *
 *	(C)opyright 1988,  all right reserved
 *		Feb.11, 1988	  by K.Osada
 *		Dec.21, 1988	  by S.Yamane
 *		Apr.24, 1989	  by K.Osada
 *  	Dec.02, 1990  	  by K.Osada
 *
 *	$Header: ORBIT.Cv  1.5  90/12/10 09:51:50  yamane  Rel $
 */

#include	<math.h>

#include	"astrolib.h"
#include	"eph.h"

#define		LIMIT			( 1.0e-9 )
#define		LIMIT_ROUGH		( 1.0e-6 )

/*
 *	elipse ---
 *		Kepler's equation
 *
 *	Feb.11, 1988   by K.Osada
 *	Nov.10, 1988   by K.Osada
 *	Dec.02, 1988   by K.Osada
 *	Dec.08, 1988   by K.Osada
 */
double
elipse( orb_el, interval, r_cosv, r_sinv, r_obj )
Orbit_element	orb_el;					/* input */
double		interval;					/* input */
double		*r_cosv, *r_sinv, *r_obj;	/* output */
{
	int		i=0;
	double	m,	w1,	w2,	ecc_anomaly,	limit;

	limit = ( ctrl_sw.simple_sw ) ? LIMIT_ROUGH : LIMIT;
	m = orb_el.daily_motion * interval;
	w2 = m + sin( atan( orb_el.ecc*sin(m)/(1.0-orb_el.ecc*cos(m)) ) );
	do {
		w1 = w2;
		if ( i++ < 100 ) {
			w2 = w1 - (w1-orb_el.ecc*sin(w1)-m)/(1.0-orb_el.ecc*cos(w1));
		} else {
			w2 = m + orb_el.ecc * sin(w1);
		}
	} while ( fabs( w2 - w1 ) > limit );

	ecc_anomaly = w2;

	*r_cosv = orb_el.sm_axis * ( cos( ecc_anomaly ) - orb_el.ecc );
	*r_sinv = orb_el.sm_axis * sqrt( 1.0 - orb_el.ecc * orb_el.ecc )
							 * sin( ecc_anomaly );
	*r_obj  = orb_el.sm_axis * ( 1.0 - orb_el.ecc * cos( ecc_anomaly ));

	return ( ecc_anomaly );
}


/*
 *	hyperbola ---
 *		equation of hyperbolic orbit
 *
 *	Dec.08, 1990   by K.Osada
 */
double
hyperbola( orb_el, interval, r_cosv, r_sinv, r_obj )
Orbit_element	orb_el;					/* input */
double		interval;					/* input */
double		*r_cosv, *r_sinv, *r_obj;	/* output */
{
	double	m,	z,	z1,	ecc_anomaly,	limit;

	limit = ( ctrl_sw.simple_sw ) ? LIMIT_ROUGH : LIMIT;
	m = orb_el.daily_motion * interval;
	z = (interval > 0) ? 1.0 : -1.0;
	do {
		z1 = z;
		z = z1 + (m - orb_el.ecc * sinh(z1) - z1)
			/ (1.0 + orb_el.ecc * cosh(z1));
	} while (fabs(z1 - z) > limit);

	ecc_anomaly = z;

	*r_cosv = orb_el.sm_axis * ( orb_el.ecc - cosh( ecc_anomaly ) );
	*r_sinv = orb_el.sm_axis * sqrt( orb_el.ecc * orb_el.ecc - 1.0 )
							 * sinh( ecc_anomaly );
	*r_obj  = orb_el.sm_axis * ( orb_el.ecc * cosh( ecc_anomaly ) - 1.0 );

	return ( ecc_anomaly );
}


/*
 *	parabola ---
 *		equation of parabolic orbit
 *
 *	Feb.11, 1988   by K.Osada
 */
double
parabola( n0 )
double	n0;		/* input */
{
	double	n1 = 0.0,	tw,	limit;
   
	limit = ( ctrl_sw.simple_sw ) ? LIMIT_ROUGH : LIMIT;
	tw = n0;
	while ( fabs( n1 - tw ) > limit ) {
		n1 = tw;
		tw = ( 2.0 / 3.0 *n1*n1*n1 + n0 ) / ( 1.0 + n1*n1 );
	}

	return ( tw );
}


/*
 *	n_parabola ---
 *		equation of nearly parabolic orbit
 *
 *	Feb.11, 1988   by K.Osada
 *	Dec.02, 1990   by K.Osada
 */
double
n_parabola( orb_el, interval, r_cosv, r_sinv, r_obj )
Orbit_element	orb_el;		/* input */
double		interval;	/* input */
double		*r_cosv, *r_sinv, *r_obj;	/* output */
{
	double	a0,	a1, a2, a3, a4, a5, a6, a7;
	double	b,	c,	d,	e;
	double	n0,	tw,	limit;

	a1 = a2 = a3 = a4 = a5 = a6 = a7 = 0.0;

	limit = ( ctrl_sw.simple_sw ) ? LIMIT_ROUGH : LIMIT;
	e = 1.0 + 9.0 * orb_el.ecc;
	n0 = sqrt( e/10.0 ) * GAUSS / sqrt( 2.0 ) * interval 
					/ ( orb_el.peri_dis * sqrt( orb_el.peri_dis ));
	do {
		a0 = a1;
		b = 1.0 - 0.017142857*a2 - 0.003809524*a3 - 0.001104267*a4
				- 0.000367358*a5 - 0.000131675*a6 - 0.000049577*a7;
		tw = parabola( b*n0 );
		a1 = 5.0 * ( 1.0 - orb_el.ecc ) / e * tw*tw;
		a2 = a1*a1;	a3 = a1*a2;	a4 = a1*a3;
		a5 = a1*a4;	a6 = a1*a5;	a7 = a1*a6;

		if( 0.00036155*a7 > 1.0e-8 ) {
			elipse( orb_el, interval, r_cosv, r_sinv, r_obj );
			return(0.0);
		}
	} while ( fabs( a1 - a0 ) > limit );

	c = 1.0 + 0.4*a1 + 0.21714286*a2 + 0.12495238*a3 + 0.07339814*a4
				     + 0.04351610*a5 + 0.02592289*a6 + 0.01548368*a7;

	d = 1.0 - a1 + 0.2*a2 + 0.00571429*a3 + 0.00495238*a4 + 0.00132888*a5
                          + 0.00048295*a6 + 0.00017652*a7;

	tw = sqrt( 5.0 * ( 1.0 + orb_el.ecc ) / e ) * c * tw;

	*r_cosv = orb_el.peri_dis * d * ( 1.0 - tw * tw );
	*r_sinv = 2.0 * orb_el.peri_dis * d * tw;
	*r_obj = orb_el.peri_dis * d * ( 1.0 + tw * tw );

	return ( tw );
}
/* ******************  end of orbit.c  *************** */
