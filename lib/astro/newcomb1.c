/*
 *	newcomb0() : Equatorial Rectangular Coordinates of the Sun (date)
 *	newcomb1() : B1950 or J2000
 *
 *	Program: Coded by S.Yamane
 *
 *	Jan. 28,  1988 (ver1.10)
 *  Nov. 25, 1990   K.Osada
 *  Dec. 02, 1990   K.Osada
 *
 *	$Header: NEWCOMB1.Cv  1.6  90/12/10 09:51:48  yamane  Rel $
 */

#include	<math.h>
#include	"astrolib.h"
#include	"eph.h"

#define		EndOfData		( -9999 )
#define		SEC2RAD(x)		( DEG2RAD( SEC2DEG( (x) ) ) )
#define		RAD2SEC(x)		( DEG2SEC( RAD2DEG( (x) ) ) )

#ifdef	MOON
#	undef	MOON
#endif
#define	MOON	EARTH

static struct {
	int				pl_No;			/* planet number */
	unsigned int	flg;
	int				i;
	int				j;
	float			s_l;			/* L   s  */
	float			K_l;			/* L   K  */
	float			s_r;			/* R   s  */
	float			K_r;			/* R   K  */
	float			s_b;			/* B   s  */
	float			K_b;			/* B   K  */
} perturb[] = {
/*                           ----  L  ----   ----  R  ----   ----  B  ----  */
/*     Planet   flg   i   j    s       K        s       K       s       K   */
	{ MERCURY, 0110,  1, -1, 0.013, 243.000,   28.0, 335.000, 0.000,   0.000 },
	{ MERCURY, 0110,  2, -1, 0.005, 225.000,    6.0, 130.000, 0.000,   0.000 },
	{ MERCURY, 0110,  3, -1, 0.015, 357.000,   18.0, 267.000, 0.000,   0.000 },
	{ MERCURY, 0110,  4, -1, 0.023, 326.000,    5.0, 239.000, 0.000,   0.000 },

	{ VENUS,   0111,  0, -1, 0.075, 296.600,   94.0, 205.000, 0.029, 145.000 },
	{ VENUS,   0111,  1, -1, 4.8328, 299.1017, 2356.0, 209.080, 0.005, 323.000 },
	{ VENUS,   0111,  2, -1, 0.074, 207.900,   69.0, 348.500, 0.092,  93.700 },
	{ VENUS,   0111,  3, -1, 0.009, 249.000,   16.0, 330.000, 0.007, 262.000 },
	{ VENUS,   0110,  0, -2, 0.0032, 162.000,    4.0,  90.000, 0.000,   0.000 },
	{ VENUS,   0111,  1, -2, 0.116, 148.900,  160.0,  58.400, 0.023, 173.000 },
	{ VENUS,   0111,  2, -2, 5.5188, 148.3133, 6832.9,  58.3183, 0.012, 149.000 },
	{ VENUS,   0111,  3, -2, 2.4950, 315.9433,  867.3, 226.700, 0.067, 123.000 },
	{ VENUS,   0111,  4, -2, 0.044, 311.400,   52.0,  38.800, 0.014, 111.000 },
	{ VENUS,   0111,  2, -3, 0.0130, 176.000,   21.0,  90.000, 0.014, 201.000 },
	{ VENUS,   0111,  3, -3, 0.6655, 177.710, 1043.9,  87.570, 0.008, 187.000 },
	{ VENUS,   0111,  4, -3, 1.5572, 345.2533, 1495.4, 255.250, 0.210, 151.800 },
	{ VENUS,   0111,  5, -3, 1.0233, 318.150,  194.0,  49.500, 0.007, 153.000 },
	{ VENUS,   0111,  6, -3, 0.017, 315.000,   19.0,  43.000, 0.004, 296.000 },
	{ VENUS,   0111,  3, -4, 0.0032, 198.000,    6.0,  90.000, 0.006, 232.000 },
	{ VENUS,   0110,  4, -4, 0.210, 206.200,  376.0, 116.280, 0.000,   0.000 },
	{ VENUS,   0111,  5, -4, 0.144, 195.400,  196.0, 105.200, 0.031,   1.800 },
	{ VENUS,   0111,  6, -4, 0.152, 343.800,   94.0, 254.800, 0.012, 180.000 },
	{ VENUS,   0110,  7, -4, 0.006, 322.000,    6.0,  59.000, 0.000,   0.000 },
	{ VENUS,   0110,  5, -5, 0.084, 235.600,  163.0, 145.400, 0.000,   0.000 },
	{ VENUS,   0111,  6, -5, 0.037, 221.800,   59.0, 132.200, 0.009,  27.000 },
	{ VENUS,   0111,  7, -5, 0.123, 195.300,  141.0, 105.400, 0.019,  18.000 },
	{ VENUS,   0110,  8, -5, 0.1540, 359.600,   26.0, 270.000, 0.000,   0.000 },
	{ VENUS,   0001,  5, -6, 0.000,   0.000,    0.0,   0.000, 0.006, 288.000 },
	{ VENUS,   0110,  6, -6, 0.038, 264.100,   80.0, 174.300, 0.000,   0.000 },
	{ VENUS,   0111,  7, -6, 0.014, 253.000,   25.0, 164.000, 0.004,  57.000 },
	{ VENUS,   0111,  8, -6, 0.010, 230.000,   14.0, 135.000, 0.004,  57.000 },
	{ VENUS,   0110,  9, -6, 0.014,  12.000,   12.0, 284.000, 0.000,   0.000 },
	{ VENUS,   0110,  7, -7, 0.020, 294.000,   42.0, 203.500, 0.000,   0.000 },
	{ VENUS,   0110,  8, -7, 0.006, 279.000,   12.0, 194.000, 0.000,   0.000 },
	{ VENUS,   0110,  9, -7, 0.003, 288.000,    4.0, 166.000, 0.000,   0.000 },
	{ VENUS,   0010, 10, -7, 0.000,   0.000,    4.0, 135.000, 0.000,   0.000 },
	{ VENUS,   0110,  8, -8, 0.011, 322.000,   24.0, 234.000, 0.000,   0.000 },
	{ VENUS,   0010,  9, -8, 0.000,   0.000,    6.0, 218.000, 0.000,   0.000 },
	{ VENUS,   0111, 12, -8, 0.042, 259.200,   44.0, 169.700, 0.010,  61.000 },
	{ VENUS,   0010, 13, -8, 0.000,   0.000,   12.0, 222.000, 0.000,   0.000 },
	{ VENUS,   0110, 14, -8, 0.032,  48.800,   33.0, 138.700, 0.000,   0.000 },
	{ VENUS,   0110,  9, -9, 0.006, 351.000,   13.0, 261.000, 0.000,   0.000 },
	{ VENUS,   0010, 10, -9, 0.000,   0.000,    4.0, 256.000, 0.000,   0.000 },
	{ VENUS,   0110, 10,-10, 0.0032,  18.000,    8.0, 293.000, 0.000,   0.000 },

	{ MARS,    0110, -2,  1, 0.006, 218.000,    8.0, 130.000, 0.000,   0.000 },
	{ MARS,    0110, -1,  1, 0.273, 217.700,  150.0, 127.700, 0.000,   0.000 },
	{ MARS,    0110,  0,  1, 0.048, 260.300,   28.0, 347.000, 0.000,   0.000 },
	{ MARS,    0110, -3,  2, 0.041, 346.000,   52.0, 255.400, 0.000,   0.000 },
	{ MARS,    0111, -2,  2, 2.0401, 343.8883, 2054.2, 253.8283, 0.008,  90.000 },
	{ MARS,    0110, -1,  2, 1.7669, 200.4016,  151.0, 295.000, 0.000,   0.000 },
	{ MARS,    0111,  0,  2, 0.028, 148.000,   31.0, 234.300, 0.008, 346.000 },
	{ MARS,    0110, -4,  3, 0.004, 284.000,    6.0, 180.000, 0.000,   0.000 },
	{ MARS,    0110, -3,  3, 0.129, 294.200,  168.0, 203.500, 0.000,   0.000 },
	{ MARS,    0110, -2,  3, 0.4236, 338.880,  215.0, 249.000, 0.000,   0.000 },
	{ MARS,    0110, -1,  3, 0.008,   7.000,    6.0,  90.000, 0.000,   0.000 },
	{ MARS,    0110, -4,  4, 0.034,  71.000,   49.0, 339.700, 0.000,   0.000 },
	{ MARS,    0111, -3,  4, 0.4995, 105.180,  476.7,  15.170, 0.007, 188.000 },
	{ MARS,    0110, -2,  4, 0.5841, 334.060,  105.0,  65.900, 0.000,   0.000 },
	{ MARS,    0110, -1,  4, 0.009, 325.000,   10.0,  53.000, 0.000,   0.000 },
	{ MARS,    0110, -5,  5, 0.007, 172.000,   12.0,  90.000, 0.000,   0.000 },
	{ MARS,    0110, -4,  5, 0.085,  54.600,  107.0, 324.600, 0.000,   0.000 },
	{ MARS,    0110, -3,  5, 0.204, 100.800,   89.0,  11.000, 0.000,   0.000 },
	{ MARS,    0110, -2,  5, 0.003,  18.000,    3.0, 108.000, 0.000,   0.000 },
	{ MARS,    0010, -6,  6, 0.000,   0.000,    5.0, 217.000, 0.000,   0.000 },
	{ MARS,    0110, -5,  6, 0.020, 186.000,   30.0,  95.700, 0.000,   0.000 },
	{ MARS,    0110, -4,  6, 0.154, 227.400,  139.0, 137.300, 0.000,   0.000 },
	{ MARS,    0110, -3,  6, 0.101,  96.300,   27.0, 188.000, 0.000,   0.000 },
	{ MARS,    0110, -6,  7, 0.006, 301.000,   10.0, 209.000, 0.000,   0.000 },
	{ MARS,    0110, -5,  7, 0.049, 176.500,   60.0,  86.200, 0.000,   0.000 },
	{ MARS,    0110, -4,  7, 0.106, 222.700,   38.0, 132.900, 0.000,   0.000 },
	{ MARS,    0110, -7,  8, 0.003,  72.000,    5.0, 349.000, 0.000,   0.000 },
	{ MARS,    0110, -6,  8, 0.010, 307.000,   15.0, 217.000, 0.000,   0.000 },
	{ MARS,    0110, -5,  8, 0.052, 348.900,   45.0, 259.700, 0.000,   0.000 },
	{ MARS,    0110, -4,  8, 0.021, 215.200,    8.0, 310.000, 0.000,   0.000 },
	{ MARS,    0110, -7,  9, 0.004,  57.000,    6.0, 329.000, 0.000,   0.000 },
	{ MARS,    0110, -6,  9, 0.028, 298.000,   34.0, 208.100, 0.000,   0.000 },
	{ MARS,    0110, -5,  9, 0.062, 346.000,   17.0, 257.000, 0.000,   0.000 },
	{ MARS,    0110, -7, 10, 0.005,  68.000,    8.0, 337.000, 0.000,   0.000 },
	{ MARS,    0110, -6, 10, 0.019, 111.000,   15.0,  23.000, 0.000,   0.000 },
	{ MARS,    0100, -5, 10, 0.005, 338.000,    0.0,   0.000, 0.000,   0.000 },
	{ MARS,    0110, -7, 11, 0.017,  59.000,   20.0, 330.000, 0.000,   0.000 },
	{ MARS,    0110, -6, 11, 0.044, 105.900,    9.0,  21.000, 0.000,   0.000 },
	{ MARS,    0110, -7, 12, 0.006, 232.000,    5.0, 143.000, 0.000,   0.000 },
	{ MARS,    0110, -8, 13, 0.013, 184.000,   15.0,  94.000, 0.000,   0.000 },
	{ MARS,    0110, -7, 13, 0.045, 227.800,    5.0, 143.000, 0.000,   0.000 },
	{ MARS,    0110, -9, 15, 0.021, 309.000,   22.0, 220.000, 0.000,   0.000 },
	{ MARS,    0110, -8, 15, 0.000,   0.000,    6.0, 261.000, 0.000,   0.000 },
	{ MARS,    0110,-10, 17, 0.004, 243.000,    4.0, 153.000, 0.000,   0.000 },
	{ MARS,    0100, -9, 17, 0.026, 113.000,    0.0,   0.000, 0.000,   0.000 },

	{ JUPITER, 0110, -3,  1, 0.003, 198.000,    5.0, 112.000, 0.000,   0.000 },
	{ JUPITER, 0111, -2,  1, 0.163, 198.600,  208.0, 112.000, 0.007, 180.000 },
	{ JUPITER, 0111, -1,  1, 7.208, 179.532, 7067.0,  89.545, 0.017, 273.000 },
	{ JUPITER, 0111,  0,  1, 2.600, 263.217,  244.0, 338.600, 0.016, 180.000 },
	{ JUPITER, 0111,  1,  1, 0.073, 276.300,   80.0,   6.500, 0.023, 268.000 },
	{ JUPITER, 0110, -3,  2, 0.069,  80.800,  103.0, 350.500, 0.000,   0.000 },
	{ JUPITER, 0110, -2,  2, 2.731,  87.145, 4026.0, 357.108, 0.000,   0.000 },
	{ JUPITER, 0111, -1,  2, 1.610, 109.493, 1459.0,  19.467, 0.166, 265.500 },
	{ JUPITER, 0110,  0,  2, 0.073, 252.600,    8.0, 263.000, 0.000,   0.000 },
	{ JUPITER, 0110, -4,  3, 0.005, 158.000,    9.0,  69.000, 0.000,   0.000 },
	{ JUPITER, 0110, -3,  3, 0.164, 170.500,  281.0,  81.200, 0.000,   0.000 },
	{ JUPITER, 0111, -2,  3, 0.556,  82.650,  803.0, 352.560, 0.006, 171.000 },
	{ JUPITER, 0111, -1,  3, 0.210,  98.500,  174.0,   8.600, 0.018, 267.000 },
	{ JUPITER, 0110, -4,  4, 0.016, 259.000,   29.0, 170.000, 0.000,   0.000 },
	{ JUPITER, 0110, -3,  4, 0.044, 168.200,   74.0,  79.900, 0.000,   0.000 },
	{ JUPITER, 0110, -2,  4, 0.080,  77.700,  113.0, 347.700, 0.000,   0.000 },
	{ JUPITER, 0110, -1,  4, 0.023,  93.000,   17.0,   3.000, 0.000,   0.000 },
	{ JUPITER, 0010, -5,  5, 0.000,   0.000,    3.0, 252.000, 0.000,   0.000 },
	{ JUPITER, 0110, -4,  5, 0.005, 259.000,   10.0, 169.000, 0.000,   0.000 },
	{ JUPITER, 0110, -3,  5, 0.007, 164.000,   12.0,  76.000, 0.000,   0.000 },
	{ JUPITER, 0110, -2,  5, 0.009,  71.000,   14.0, 343.000, 0.000,   0.000 },

	{ SATURN,  0110, -2,  1, 0.011, 105.000,   15.0,  11.000, 0.000,   0.000 },
	{ SATURN,  0111, -1,  1, 0.419, 100.580,  429.0,  10.600, 0.006, 260.000 },
	{ SATURN,  0110,  0,  1, 0.320, 269.460,    8.0, 353.000, 0.000,   0.000 },
	{ SATURN,  0111,  1,  1, 0.008, 270.000,    8.0,   0.000, 0.006, 280.000 },
	{ SATURN,  0010, -3,  2, 0.000,   0.000,    3.0, 198.000, 0.000,   0.000 },
	{ SATURN,  0110, -2,  2, 0.108, 290.600,  162.0, 200.600, 0.000,   0.000 },
	{ SATURN,  0110, -1,  2, 0.112, 293.600,  112.0, 203.600, 0.034, 300.000 },
	{ SATURN,  0100,  0,  2, 0.017, 227.000,    0.0,   0.000, 0.000,   0.000 },
	{ SATURN,  0110, -2,  3, 0.021, 289.000,   32.0, 200.100, 0.000,   0.000 },
	{ SATURN,  0110, -1,  3, 0.017, 291.000,   17.0, 201.000, 0.000,   0.000 },
	{ SATURN,  0110, -2,  4, 0.003, 288.000,    4.0, 194.000, 0.000,   0.000 },
   
	{ EndOfData }
};
static double	 G[ SATURN+1 ];				/*  mean anomary [sec]	  */
static double	 G1;					/*  mean anomary [rad]	  */

static void	by_planet(double T, double *dL2, double *dR2, double *dB2)
{
	register int	i,	wf;
	double			w;

/*
 *	mean anomary for each planets [deg]
 */
	G[ MERCURY ] =  102.20 + 149472.350*T;
	G[ VENUS   ] =  212.45 +  58517.493*T;
	G[ MARS    ] =  319.58 +  19139.977*T;
	G[ JUPITER ] =  225.28 + 3034.583*T 
				 + ( 0.332 - 0.0015*T ) * sin( DEG2RAD( 133.91+38.394*T ) );
	G[ SATURN  ] =  175.60 +   1221.794*T;

/*
 *	priodic perturbation by planets
 */
	*dL2 =  *dR2 = *dB2 = 0.0;
	for ( i=0;  perturb[i].pl_No != EndOfData;  i++ ) {
		wf =  perturb[i].flg;
		w  =  (double)(perturb[i].i) * G[ OUR_SUN ]
			+ (double)(perturb[i].j) * G[ perturb[i].pl_No ];
		if ( wf & 0100 )	/*  calculate L */
			*dL2 += ( perturb[i].s_l * cos( DEG2RAD( perturb[i].K_l - w ) ) );
		if ( wf & 0010 )	/*  calculate R  */
			*dR2 += ( perturb[i].s_r * cos( DEG2RAD( perturb[i].K_r - w ) ) );
		if ( wf & 0001 )	/*  calculate B */
			*dB2 += ( perturb[i].s_b * cos( DEG2RAD( perturb[i].K_b - w ) ) );
	}
}

static void  by_moon(double T, double T2, double T3, double *dL3, double *dR3, double *dB3)
{
	double	D,	U,	Udash;
	double	w,	w1,	w2,	w3,	w4,	w5,	w6,	w7,	w8,	w9;

/*
 *	some angles for moon [sec]
 */
	D		  = 1262654.95 + 1602961611.18*T -  5.1700*T2 + 0.0068*T3;
	G[ MOON ] = 1065976.59 + 1717915856.79*T + 33.0900*T2 + 0.0518*T3;
	U		  =   40503.20 + 1739527290.54*T - 11.5600*T2 - 0.0012*T3;
	Udash	  =   73848.25 +  136565679.36*T -  6.3910*T2 - 0.0080*T3;

/*
 *	perturbation by moon
 */
	w  = SEC2RAD( G[MOON] );
	w1 = SEC2RAD( D );
	w2 = SEC2RAD( D*3.0 );
	w3 = w1 + w;	w4 = w1 - w;
	w5 = w2 - w;
	w6 = w1 + G1;   w7 = w1 - G1;
	w8 = w4 - G1;
	w9 = SEC2RAD( Udash*2.0 );
   *dL3  =  6.454   * sin( w1 )  + 0.013 * sin( w2 )  + 0.177 * sin( w3 )
		  - 0.424   * sin( w4 )  + 0.039 * sin( w5 )  - 0.064 * sin( w6 )
		  + 0.172   * sin( w7 )  - 0.013 * sin( w8 )  - 0.013 * sin( w9 );
   *dR3  =  13360.0 * cos( w1 )  +  30.0 * cos( w2 )  + 370.0 * cos( w3 )
		  -  1330.0 * cos( w4 )  +  80.0 * cos( w5 )  - 140.0 * cos( w6 )
		  +   360.0 * cos( w7 )  -  30.0 * cos( w8 )  +  30.0 * cos( w9 );
	w1 = SEC2RAD( U );
	w2 = w1 + w;	w3 = w1 - w;
	w4 = w1 - w9;
	w5 = w3 - w9;
	w6 = w1 + G1;   w7 = w1 - G1;
   *dB3  =  0.576   * sin( w1 )  + 0.016 * sin( w2 )  - 0.047 * sin( w3 )
		  + 0.021   * sin( w4 )  + 0.005 * sin( w5 )  + 0.005 * sin( w6 )
		  + 0.005   * sin( w7 );

}

/*
 *	newcomb_d2B ---
 *		convert XYZ date to B1950
 */
Xyz
newcomb_d2B( jd, XYZdate, XYZ1950 )
double	jd;			/* Input  */
Xyz		XYZdate;		/* Input  */
Xyz		*XYZ1950;		/* Output */
{
	double  y1, y2, y3, y4, y5;
	double  xx, xy, xz;
	double  yx, yy, yz;
	double  zx, zy, zz;

	y1 = ( jd-2433282.423 ) / 36524.22;
	y2 = y1 * y1;
	y3 = y2 * y1;
	y4 = y3 * y1;
	y5 = y4 * y1;

	xx =   1.0				-  0.0002969571 * y2  -  0.0000001311 * y3
		+  0.0000000146 * y4 ;
	xy =   0.0223494065 * y1  +  0.0000067631 * y2  -  0.0000022082 * y3
		-  0.0000000017 * y4  +  0.0000000001 * y5 ;
	xz =   0.0097169024 * y1  -  0.0000020653 * y2  -  0.0000009613 * y3
		+  0.0000000001 * y4 ;
	yx =  -0.0223494065 * y1  -  0.0000067632 * y2  +  0.0000022083 * y3
		+  0.0000000015 * y4  -  0.0000000001 * y5 ;
	yy =   1.0				-  0.0002497480 * y2  -  0.0000001512 * y3
		+  0.0000000122 * y4 ;
	yz =						-0.0001085835 * y2  +  0.0000000089 * y3
		+  0.0000000054 * y4 ;
	zx =  -0.0097169024 * y1  +  0.0000020653 * y2  +  0.0000009613 * y3
		+  0.0000000003 * y4 ;
	zy =						-0.0001085835 * y2  -  0.0000000284 * y3
		+  0.0000000053 * y4 ;
	zz =   1.0				-  0.0000472091 * y2  +  0.0000000201 * y3
		+  0.0000000023 * y4 ;

	XYZ1950->x = xx * XYZdate.x  +  xy * XYZdate.y  +  xz * XYZdate.z;
	XYZ1950->y = yx * XYZdate.x  +  yy * XYZdate.y  +  yz * XYZdate.z;
	XYZ1950->z = zx * XYZdate.x  +  zy * XYZdate.y  +  zz * XYZdate.z;

	return ( *XYZ1950 );
}

/*
 *	newcomb0 ---
 *		calculate SUN XYZ (date)
 */
Xyz
newcomb0( jd, XYZdate,  LBRt )	/* return : date mean */
double	jd;				/* input  : JDE		  */
Xyz	*XYZdate;			/* output : date mean */
Lbr	*LBRt;				/* L/R/B at t */
{
	double	sin_ecl,	cos_ecl,	sin_L,	cos_L,	sin_B,	cos_B;
	double	T,	T2,	T3;							/*  Julian century */
	double	L0,	dL1,	dL2,	dL3,	dL4;	/*  黄経 */
	double			dB2,	dB3;				/*  黄緯 */
	double	dR1,	dR2,	dR3;				/*  動径の常用対数値 */
	double  ecl;								/*  ε [rad] */
	double  p1,	p2,	p3,	p4;

/*
 *	Julian century
 */
	T  = ( jd - 2415020.0 ) / 36525.0;
	T2 = T  * T;
	T3 = T2 * T;

/*
 *	平均黄経 [sec]
 */
	L0 = 1006908.04 + 129602768.13*T + 1.0890*T2;

/*
 *	mean anomary [deg] & [rad]
 */
	p1 = DEG2RAD(  57.24+150.27*T );
	p2 = DEG2RAD( 231.19+ 20.20*T );
	p3 = DEG2RAD(  31.80+119.00*T );
	G[ OUR_SUN ]  =  1290513.00 + 129596579.10*T - 0.5400*T2 - 0.0120*T3
				   + ( 1.882 - 0.016*T ) * sin( p1 )
				   +		   6.40	  * sin( p2 )
				   +		   0.266	 * sin( p3 ) ;
	G[ OUR_SUN ]  =  SEC2DEG( G[ OUR_SUN ] );
	G1 = DEG2RAD( G[ OUR_SUN ] );


/*
 *	平均黄経における永年摂動 [sec]
 */
	p4 = DEG2RAD( 315.60 + 893.30*T );
	dL4  =  (1.882-0.016*T) * sin( p1 )
		  +		6.400	* sin( p2 )
		  +		0.266	* sin( p3 )
		  +		0.202	* sin( p4 );

/*
 *	黄経における中心差 [sec]
 */
	dL1 =  ( 6910.057 - 17.242*T - 0.0520*T2 ) * sin( G1 )
		 + (   72.339 -  0.361*T )			 * sin( G1 * 2.0 )
		 + (	1.050 -  0.008*T )			 * sin( G1 * 3.0 )
		 +	  0.017						  * sin( G1 * 4.0 );

/*
 *	動径の常用対数値
 */
	dR1  =  (	30570.0 -   150.0*T )
		  + ( -7274120.0 + 18150.0*T + 60.0*T2 ) * cos( G1 )
		  + ( -  91380.0 +   460.0*T )		   * cos( G1 * 2.0 )
		  + ( -   1450.0 +	10.0*T )		   * cos( G1 * 3.0 )
		  -		 20.0						 * cos( G1 * 4.0 );

/*
 *	perturbation by planets
 */
	by_planet( T, &dL2, &dR2, &dB2 );

/*
 *	perturbation by moon
 */
	by_moon( T, T2, T3, &dL3, &dR3, &dB3 );

/*
 *	黄道傾斜角 [rad]
 */
	ecl = obl_ecl( jd );

/*
 *	時刻 t における黄経・黄緯・動径 [rad]
 */
	LBRt->l =  SEC2RAD( L0 + dL1 + dL2 + dL3 + dL4 );
	LBRt->b =  SEC2RAD( -dB2 + dB3 );
	LBRt->r =  pow( 10.0,  (dR1+dR2+dR3)*(1.0e-9) );
/*
 *	時刻 t における平均分点による赤道直角座標
 */
	sin_ecl = sin( ecl );		 cos_ecl = cos( ecl );
	sin_L = sin( LBRt->l );	cos_L = cos( LBRt->l );
	sin_B = sin( LBRt->b );	cos_B = cos( LBRt->b );
	XYZdate->x =  LBRt->r *   cos_L * cos_B;
	XYZdate->y =  LBRt->r * ( cos_ecl * sin_L * cos_B - sin_ecl * sin_B );
	XYZdate->z =  LBRt->r * ( sin_ecl * sin_L * cos_B + cos_ecl * sin_B );

	return ( *XYZdate );
}

/*
 *	newcomb1 ---
 *		calculate SUN XYZ (B1950.0)
 */
Xyz
newcomb1( jd, XYZ, LBRt )
double	jd;				/* input  : JED		  */
Xyz	*XYZ;				/* output : B1950 or J2000  */
Lbr	*LBRt;				/* L/R/B */
{
	Xyz		XYZdate;
	Xyz		XYZ1950;

	XYZdate = newcomb0( jd, &XYZdate, LBRt );	/* XYZ as date mean */
	newcomb_d2B( jd, XYZdate, &XYZ1950 );		/* XYZ as B1950 */

	if ( ctrl_sw.orb_el_system == B1950_IN ) {
		*XYZ = XYZ1950;
	} else if ( ctrl_sw.orb_el_system == J2000_IN ) {
		xyzb2j( XYZ1950, XYZ );					/* XYZ as J2000 */
	}

	return ( *XYZ );
}


/*
 *	newcombs ---
 *
 *	Program : Coded by K.Osada
 *		Jan. 23,  1989
 *		Nov. 25,  1990
 */
void
newcombs( jd, XYZ1950, lbr )
double	jd;				/* input  : JED	   */
Xyz	*XYZ1950;			/* output : B1950  */
Lbr	*lbr;
{
	double	m,	l,	r;
	double	sin_l,	cos_l;

	m = fmod( DEG2RAD( 315.2526 + 0.985600267*(jd-2400000.5) ), PIX2 );
	l = fmod( DEG2RAD( 237.0388 + 0.985609104*(jd-2400000.5)
		+ 1.9159*sin( m ) + 0.0201*sin( 2.0*m ) ), PIX2 );
	r = 1.00014 - 0.01672*cos( m ) - 0.00014*cos( 2.0*m );

	if ( ctrl_sw.orb_el_system == J2000_IN ) {
		l = l + DEG2RAD(0.6984);					/* l as J2000 */
	}

	sin_l = sin( l );
	cos_l = cos( l );

	XYZ1950->x = r * cos_l;
	XYZ1950->y = ECL_COS1950 * r * sin_l;
	XYZ1950->z = ECL_SIN1950 * r * sin_l;

	lbr->l = l;
	lbr->b = 0.0;
	lbr->r = r;
}

/* **************  end of newcomb1.c  ***************** */
