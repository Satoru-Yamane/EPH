/*
 *	time_sys.c :
 *		time system routines for EPH
 *
 *	(C)opyright 1988-1990  all right reserved
 *		Mar.25, 1988	  by K.Osada
 *		Dec.21, 1988	  by S.Yamane
 *		Apr.16, 1989	  by K.Osada
 *		Dec.21, 1989	  by K.Osada
 *		Dec.06, 1990	  by K.Osada
 *		Feb.26, 2012	  by S.Yamane (ut2et())
 *
 *	$Header: TIME_SYS.Cv  1.9  91/03/11 10:27:42  yamane  Rel $
 */

#include	<math.h>
#include	<time.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"getval.h"

#if defined(_WIN32)
  #define	PATHDELIM	"\\"
  #define	strtok_r(s, d, b)	strtok(s, d)
#else
  #define	PATHDELIM	"/"
#endif

/*
 *	gst_date ---
 *
 *	Apr.04, 1988
 */
double
gst_date( jd )
double	jd;		/* input */
{
	double	t,	wk,	wk1;
	double	gr_sid_t;		/* return value (rad) */

/*
	t = ( jd - 2415020.0 ) / 36525.0;
	wk = ( 6.0 + ( 38.0 + ( 45.836 / 60.0 ) ) / 60.0
		   + 8640184.542  / 60.0 / 60.0 * t
		   +	   0.0929 / 60.0 / 60.0 * t * t ) / 24.0;
	gr_sid_t = PIX2 * modf( wk , &wk1 );
*/

	t = ( jd - JD2000 ) / 36525.0;
	wk = ( 6.0 + ( 41.0 + ( 50.54841 / 60.0 ) ) / 60.0
		   + 8640184.812866  / 60.0 / 60.0 * t
		   +	   0.093104 / 60.0 / 60.0 * t * t
		   +	   0.0000062 / 60.0 / 60.0 * t * t * t
		  ) / 24.0;
	gr_sid_t = PIX2 * modf( wk , &wk1 );

	return ( gr_sid_t );
}


/*
 *	time2day ---
 *		convert int day,hour,minute,sec*100 to double day
 */
double
time2day( h, m, cs )
int	h,	m,	cs;
{
	double	ret;

	ret =  (double)h  /   24.0;
	ret += (double)m  / ( 24.0 * 60.0 );
	ret += (double)cs / ( 24.0 * 60.0 * 60.0 ) / 100.0;

	return ( ret );
}


/*
 *	day2time ---
 *		convert double day to int day,hour,minute,sec*100
 */
int
day2time( d, h, m, cs )
double	d;
int	*h,	*m,	*cs;
{
	int		ret;
	double	w,	ival;


/*
	d += 1.0e-8;
*/
	w = modf( d, &ival ) * 24.0;		ret = (int)ival;
	w = modf( w, &ival ) * 60.0;		*h = (int)ival;
	w = modf( w, &ival ) * 60.0;		*m = (int)ival;
	*cs = (int)( w * 100.0 + 0.5 );

	return ( ret );
}

/*
 *	day2jd ---
 *		day -> jd conversion
 *
 *	by	K.Osada
 *
 *	Dec. 5,  1990
 */
double
day2jd( tm )
Time	tm;		/* input */
{
	double	jd;

	if ( tm.mon <= 2 ) {
		tm.mon += 12;	tm.year--;
	}
	jd = (double)( (long)( 365.25*(double)tm.year ) )
		+ (floor)( 30.59*(double)( tm.mon - 2 ) )
		+ (double)tm.day
 		+ time2day( tm.hour, tm.min, tm.csec )
		+ 1721086.5;

	if ( jd >= 2299160.5 ) {
		jd += ( (double)( tm.year/400 - tm.year/100 ) + 2.0 );
	} else if ( tm.year < 0 ) {
		jd -= 1.0;
	}
/*
	jd -= 1.0e-8;
*/

	return ( jd );
 }


/*
 *	jd2day ---
 *		jd -> day conversion
 *
 *	by  K.Osada
 *	by  S.Yamane
 *
 *	Dec. 19,  1990
 */
double
jd2day( jd, tm )
double	jd;			/* input */
Time	*tm;			/* output */
{
	register int	year,	mon;
	int				h,	m,	cs;
	double			day,	wk,	wk2;
	Time			tm0;
	static int		days[] = {
		-1,
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
		31,			/* for next year */
	} ;


	tm0.year = (int)floor( jd / 365.2422 - 4712.0 );
	tm0.mon = 1;
	tm0.day = tm0.hour = tm0.min = tm0.csec = 0;
	day = jd - day2jd( tm0 );
	year = tm0.year;

	if ( year == 1582 && jd >= 2299160.5 ) {
		day += 10.0;
	}

	if ( jd >= 2299160.5 ) {
		if (   year % 400 == 0
		  || ( year % 100 != 0 && year % 4 == 0 ) ) {
			days[2] = 29;
		}
	} else {
		if ( year >= 0 ) {
			if ( year % 4 == 0 )  {
				days[2] = 29;
			}
		} else {
			if ( ( year-1 ) % 4 == 0 )  {
				days[2] = 29;
			}
		}
	}
	for ( mon = 1; mon <= 12; mon++ ) {
		if ( (int)day - days[mon] <= 0 )
			break;
		day -= days[mon];
	}

	if ( tm->mon == 13 ) {
		year++;
		mon -= 12;
	}
	if ( day < 1.0 ) {
		year--;
		mon = 12;
		day += 31.0;
		if ( day < 1.0 ) {
			mon = 11;
			day += 30.0;
		}
	}
	
	tm->year = year;
	tm->mon  = mon;
	tm->day  = day2time( day, &h, &m, &cs );
	tm->hour = h;
	tm->min  = m;
	tm->csec = cs;

	days[2] = 28;		/* restore */

	return ( day );
}


/*
 *	lst_date ---
 *		local sidereal time
 *
 *	by K.Osada
 *
 *	Apr. 23,  1988
 */
double
lst_date( jd )
double	jd;				/* input */
{
	double	wk,	wk1;
	double	loc_sid_t;		/* return value (rad) */

	wk = 0.7769194 + 1.002737909265 * ( jd - 2415020.0 );
	loc_sid_t = PIX2 * modf( wk, &wk1 );

	return ( loc_sid_t );
}


/*
 *	ut2et ---
 *		UT -> ET conversion
 *
 *	by K.Osada
 *
 *	Apr. 09,  1989
 *	Dec. 03,  1990
 *	Feb. 26,  2012 (change from const to file input)
 */
typedef struct {
	double	jd;
	int	dt;
} ut2et_tbl;

static ut2et_tbl	*d_et = NULL;
static int		n_d_et = 0;

int
ut2et_finish()
{
	if (d_et != NULL) {	/* in case called again */
		free(d_et);
		d_et = NULL;
		n_d_et = 0;
	}

	return (0);
}

int
ut2et_init()
{
	static char	fname[FN_LEN];
	char	*filedelim = "\t \r\n";
	int	i;
	FILE	*fp = NULL;

	int	lines;
	static char	buf[256];
	char	*ev, *brk, str_St_y[5], str_St_m[3], str_St_d[3], str_dT[9];
	int	fld_no;
	double	w_jd;
	int	w_dt;
	int	any_error = 0;
        double	jd, dT;
        Time	st_date;

  fp = fopen(current.deltaT_table, "r");
  
  if(fp == NULL) {
    return (E_FOPEN);	/* FATAL error */
    }
  
  if(d_et != NULL) {		/* in case called again */
    free(d_et);
    d_et = NULL;
    n_d_et = 0;
    }

/*
 * parse line
 */
    for(lines = 0; ! feof(fp) && ! any_error; lines++) {
      if(fgets(buf, sizeof(buf), fp) == NULL) break;
    
      if(*buf == '#'		/* comment */
    	  || *buf == '\r'
    	  || *buf == '\n'
    	  || strlen(buf) == 0) {
    	continue;
        }

      strncpy(str_St_y, buf,    4);
      strncpy(str_St_m, buf+5,  2);
      strncpy(str_St_d, buf+8,  2);
      strncpy(str_dT,   buf+11, 8);
      st_date.year = atoi(str_St_y);
      st_date.mon  = atoi(str_St_m);
      st_date.day  = atoi(str_St_d);
      st_date.hour = st_date.min = st_date.csec = 0;
      dT = atof(str_dT);

/*
      w_jd = day2jd(st_date) + 0.00000001;
      w_dt = (dT * 1000000) / 3600 / 24 + 0.4;
*/
      w_jd = day2jd(st_date);
      w_dt = (dT * 1000000) / 86400;

      d_et = (ut2et_tbl *)realloc(d_et, (n_d_et + 1) * sizeof(ut2et_tbl));
      if(d_et == NULL) {
        any_error++;
        } else {
        d_et[n_d_et].jd = w_jd;
        d_et[n_d_et].dt = w_dt;
        n_d_et++;
        }
      }
    fclose(fp);
  
    return ((any_error) ? E_INIT_UT2ET : 0);
  }

double
ut2et(jd)
double	jd;		/* input */
{
	register int	n;

	for (n = 0; n < n_d_et; n++) {
		if (jd + 1.0e-8 >= d_et[n].jd)
			return ((double)(d_et[n].dt) * 1.0e-6);
	}

	return (0.0);	/* unknown value */
}

double
et2ut(jde)
double	jde;		/* input */
{
	register int	n;

	for (n = 0; n < n_d_et; n++) {
		if (jde + 1.0e-8 >= d_et[n].jd + (d_et[n].dt) * 1.0e-6)
			return ((double)(d_et[n].dt) * 1.0e-6);
	}

	return (0.0);	/* unknown value */
}


double
time_convert( jd_in )
double	jd_in;
{
	double	jd_out, w_et2ut;

	jd_out = jd_in;

	switch ( ctrl_sw.time_system ) {
		case JST_IN:
			jd_out = UT2JST( jd_out );
			/* No break */
		case UT_IN:
			w_et2ut = et2ut(jd_out);
			if(w_et2ut == 0) {
				w_et2ut = current.ut_et;
				}
			jd_out -= ( ctrl_sw.ut2et_sw ? current.ut_et : w_et2ut );
			/* No break */
		case ET_IN:	case TT_IN:
			break;
	}
	return ( jd_out );
}


double dostime2jd(long ltime)
{
	struct tm	*stm;
	Time		tmm;

	stm = gmtime( &ltime );
	tmm.year = stm->tm_year + 1900;
	tmm.mon  = stm->tm_mon + 1;
	tmm.day  = stm->tm_mday;
	tmm.hour = stm->tm_hour;
	tmm.min  = stm->tm_min;
	tmm.csec = stm->tm_sec * 100;

	return ( day2jd( tmm ) );
}


double
jd_now()
{
	long	ltime;

	time( &ltime );

	return ( dostime2jd( ltime ) );
}
/* ********** end of time_sys.c ********** */
