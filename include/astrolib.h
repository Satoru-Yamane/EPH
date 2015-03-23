/*
 *	astrolib.h
 *		Header-file for astronomic library
 *
 *	Program
 *		Coded by  S.Yamane
 *	              K.Osada
 *	   Nov. 25, 1990	 by K.Osada
 *	   Dec. 03, 1990	 by K.Osada
 *
 *	$Header: ASTROLIB.Hv  1.9  91/01/11 16:49:38  yamane  Rel $
 */

#ifndef	_ASTROLIB_DEF_

#define	_ASTROLIB_DEF_


#define  SUCCESS		( 1 )
#define  FAILURE		( 0 )

#define  PI				( 3.1415926535897932 )	/*  ƒÎ                  */
#define  PIX2			( 2.0 * PI )			/*  2ƒÎ                 */

#define  RAD2DEG( x )	( (x)*( 180.0 / PI ) )	/*  radian to degree    */
#define  DEG2RAD( x )	( (x)*( PI / 180.0 ) )	/*  degree to radian    */
#define  SEC2DEG( d )	( (d) / 3600.0 )		/*  second to degree    */
#define  DEG2SEC( d )	( (d) * 3600.0 )		/*  degree to second    */

#define  GAUSS			( 0.01720209895  )		/*  Gaussian constant   */
#define  LIGHT_TIME		( 0.005775518308 )		/*  correct light time  */
#define  ECL1950		( DEG2RAD( 23.445788 ) ) /* obliquity of ecliptic(B1950) */
#define  ECL_SIN1950	( 0.39788118 )			/*  sin( ECL )  (B1950)  */
#define  ECL_COS1950	( 0.91743695 )			/*  cos( ECL )  (B1950)  */
#define  ECL2000		( DEG2RAD( 23.439291 ) ) /* obliquity of ecliptic(J2000) */
#define  ECL_SIN2000	( 0.39777715 )			/*  sin( ECL )  (J2000)  */
#define  ECL_COS2000	( 0.91748206 )			/*  cos( ECL )  (J2000)  */

#define  JST2UT( t )	( (t) - 0.375 )			/*  JST -> UT (in date) */
#define  UT2JST( t )	( (t) + 0.375 )			/*  UT -> JST (in date) */

/*
 *	YEAR MONTH DAY
 */
enum month {
    JAN = 1,    FEB,    MAR,    APR,    MAY,    JUN,
    JUL,        AUG,    SEP,    OCT,    NOV,    DEC,
} ;

#define  JD1950		( 2433282.423 )		/* jd of B1950.0 */
#define  JD2000		( 2451545.0 )		/* jd of j2000.0 */

/*
 *	planets in the solar system
 */
#define  OUR_SUN	( 0 )
#define  MERCURY	( 1 )
#define  VENUS		( 2 )
#define  EARTH		( 3 )
#define  MARS		( 4 )
#define  JUPITER	( 5 )
#define  SATURN		( 6 )
#define  URANUS		( 7 )
#define  NEPTUNE	( 8 )
#define  PLUTO		( 9 )


/*
 *	Orbital Element structure
 */
typedef struct _Orbit_element {
    char	roman[12];
    char	number[11];
    char	name[31];
    double	epoch;			/* jd     */
    double	mean_anomaly;		/* [rad]  */
    double	time_pass;		/* jd     */
    double	peri;			/* [rad]  */
    double	node;			/* [rad]  */
    double	incl;			/* [rad]  */
    double	sm_axis;		/* [AU]   */
    double	ecc;
    double	peri_dis;		/* [AU]   */
    double	daily_motion;		/* [rad]  */
    double	period;			/* [year] */
    double	h1;
    double	k1;
    double	h2;
    double	k2;
    double	g;
    double	tail;			/*  tail               */
} Orbit_element;


/*
 *	Ephem. Const (Px,Py,Pz) & (Qx,Qy,Qz)
 */
typedef struct _Eph_const {
    double  Px;
    double  Py;
    double  Pz;
    double  Qx;
    double  Qy;
    double  Qz;
} Eph_const;


/*
 *	Time structure (private)
 */
typedef struct _Time {
    int		year;			/*  year        */
    int		mon;			/*  month       */
    int		day;			/*  day         */
    int		hour;			/*  hour        */
    int		min;			/*  minute      */
    int		csec;			/*  sec. X 100  */
} Time;


/*
 *	coordinates
 */
typedef struct _Xyz {
    double  x, y, z;
} Xyz;

typedef struct _Lbr {
    double  l, b, r;
} Lbr;

/*
 *	function prototypes
 */
/* ***  time system  *** */
extern double	day2jd( Time );
extern double	jd2day( double, Time * );
extern double	time2day( int, int, int );
extern int	day2time( double, int *, int *, int * );
extern double	gst_date( double );
extern double	lst_date( double );
extern double	ut2et( double );
extern double	time_convert( double );

/* ***  general  *** */
extern double	elipse( Orbit_element, double, double *,
						double *, double *);
extern double	hyperbola( Orbit_element, double, double *,
						double *, double *);
extern double	n_parabola( Orbit_element, double, double *,
						double *, double *);
extern double	parabola( double );

extern void	vector_c( Orbit_element, Eph_const * );
extern void	parallax( double, double, double, double, Xyz * );
extern void	prec_b( double, double, Xyz, Xyz * );
extern Xyz	xyzb2j( Xyz, Xyz * );
extern Xyz	xyzj2b( Xyz, Xyz * );
extern void	obj_pos( Eph_const, double, double, Xyz, Xyz * );
extern void	locate( Xyz, double *, double *, double * );
extern void	eph_calc( double, Orbit_element, Eph_const, Xyz,
						Xyz *, double *, double *, double *, double * );
extern void	equ2hor( double, double, double, double, double,
						double *, double * );
extern double	obl_ecl( double );
extern void	ecl2equ( double, Xyz, Xyz * );
extern void	equ2ecl( double, Xyz, Xyz * );
extern void	sun2obj( Xyz, double, double *, double * );
extern void	lenth_pa( double, double, double, double,
						double *, double * );
extern void	ra_decl( Xyz, double *,	double * );

extern void	position( double, double, double, Xyz * );
extern double	optimum( double, Orbit_element, double, double );
extern Xyz	newcomb0( double,  Xyz *, Lbr * );
extern Xyz	newcomb1( double,  Xyz *, Lbr * );
extern Xyz	newcomb_d2B( double, Xyz, Xyz * );

extern void	moon( double, double, double *, double *, double * );

/* ***  output calculation *** */
extern double	comet_mag1( double, double, double, double );
extern double	comet_mag2( double, double, double, double, double );
extern double	mp_v_mag( double, double, double, double );
extern double	mp_p_mag( double, double, double, double, double );
extern double	position_angle( double, double, double );
extern double	elongation( double, double, double );
extern void		tail( Xyz, Xyz, double, double, double, double,
							double *, double * );
extern double	moon_age( double );
extern double	earth2tail( Eph_const, Xyz, double );
extern void	var_T( double, Orbit_element, Eph_const, double, double,
							double, double, double, double *, double * );
extern void	var_M( double, Orbit_element, Eph_const, double, double,
							double, double, double,	double *, double * );
extern void	metcalf( double, Orbit_element, Eph_const, double, double,
							double, double, double, double *, double * );
extern void	daily_m( double, Orbit_element, Eph_const, double, double,
							double, double, double,	double *, double * );

/* ***  planet system  *** */
extern void	pl_orbit( double, int,  Orbit_element *, double * );
extern double	mercu1( double, double, Orbit_element * );
extern double	venus1( double, double, Orbit_element * );
extern double	earth1( double, double, Orbit_element * );
extern double	mars1(  double, double, Orbit_element * );
extern double	jupit1( double, double, Orbit_element * );
extern double	satur1( double, double, Orbit_element * );
extern double	uranu1( double, double, Orbit_element * );
extern double	neptu1( double, double, Orbit_element * );
extern double	pluto1( double, double, Orbit_element * );

/* defined newcomb1.c */
extern void	newcombs(double jd, Xyz *, Lbr *);

/* defined moon_loc.c */
void moon_loc(double, Xyz *, double *);

#endif	/* _ASTROLIB_DEF_ */

/* ****************  end of astrolib.h  ***************** */
