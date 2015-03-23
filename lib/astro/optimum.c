/*
 *	optimum.c
 *		optimum obs. time
 *
 *	(C)opyright 1988, All right reserved
 *				by K.Osada
 *				by S.Yamane
 *
 *	$Header: OPTIMUM.Cv  1.7  90/12/14 08:58:12  yamane  Rel $
 */

#include	<math.h>
#include	<stdlib.h>

#include	"astrolib.h"
#include	"eph.h"

static double	transit(double jd, Orbit_element orb_el, double longi)
{
	double		jd0, jd1, ra, decl, delta, r_obj, transit_time;
	Eph_const	eph_c;
	Xyz			pos;
	Xyz			xyz1950;
	Lbr			lbr;

	jd1 = jd + 0.6;
	do {
		vector_c( orb_el, &eph_c);
		newcombs( jd1, &xyz1950, &lbr );	/* simple newcomb() */
		eph_calc( jd1, orb_el, eph_c, xyz1950,
					   &pos, &ra, &decl, &delta, &r_obj );
		transit_time = in_pix2( ra - ( gst_date( jd ) + longi ) );
		jd0 = jd1;
		jd1 = jd + RAD2DEG(transit_time) / 360.0;
	} while ( fabs(jd0-jd1) < 1.0e-4 );

	return ( transit_time );
}


static void	twilight(double jd, double longi, double lati, double *twilight1, double *twilight2)
{
	double	jd1, dh, ra, decl, delta;
	Xyz		pos;
	Lbr		lbr;
	Xyz		xyz1950;

	jd1 = jd + 0.6;
	newcombs( jd1, &xyz1950, &lbr );		/* simple newcomb() */
	pos.x = xyz1950.x;
	pos.y = xyz1950.y;
	pos.z = xyz1950.z;
	locate( pos, &ra, &decl, &delta );
	dh = acos( cos(DEG2RAD(108.0)) / cos( lati ) / cos ( decl )
	   - tan( lati ) * tan( decl ) );

	*twilight1 = in_pix2( (  dh + ra - (lst_date(jd) + longi) ) / 1.0027379 );
	*twilight2 = in_pix2( ( -dh + ra - (lst_date(jd) + longi) ) / 1.0027379 );
}


/*
 *	optimum ---
 *		calculate optimum observation time
 */
double
optimum( jd, orb_el, longi, lati )
double	jd;
Orbit_element	orb_el;
double	longi,	lati;
{
	double	transit_time;
	double	twilight1,	twilight2,	opt_t;
	double	opt_jde;

	transit_time = transit( jd, orb_el, longi );
	twilight( jd, longi, lati, &twilight1, &twilight2 );
	if ( twilight1 < transit_time  &&  transit_time < twilight2 ) {
		opt_t = transit_time;
	} else if ( fabs(twilight1-transit_time) > fabs(twilight2-transit_time) || 
				fabs(twilight1-transit_time) > fabs(twilight2-PIX2-transit_time) ) {
		opt_t = twilight2 - DEG2RAD( 0.00273 * 360.0 ); /* dummy */
	} else {
		opt_t = twilight1 ;
	}
	opt_jde = jd + RAD2DEG( opt_t ) / 360.0;

	return ( opt_jde );
}
/* ****************  end of optimum.c  ************* */
