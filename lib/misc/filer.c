/*
 *	filer.c :
 *		System filer for EPH
 *
 *	(C)opyright 1988-1990,  all right reserved
 *		Dec.21, 1988	  by S.Yamane
 *	  	Apr.24, 1989	  by K.Osada
 *
 *	$Header: FILER.Cv  1.15  91/04/01 22:33:36  yamane  Rel $
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<ctype.h>
#include	<stdarg.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"filer.h"
#include	"ephcurs.h"

#define  LMOD( a, b )		( (long)(a) - ( (long)(a)/(long)(b) * (long)(b) ) )

/*
 *	variables
 */
static char	read_buf[ MAX_RECSIZE ];

/*
 *	function
 */
/*
 *	level 0
 */
static int	seek_and_load(FILE *fp, int recno, int recsize, char *buf)
{
	if ( recsize > MAX_RECSIZE ) {
		return ( RECSIZE_ERROR );
	}
	if ( recno < 0 ) {
		return ( SEEK_BEFORE_TOP );
	}
	if ( fseek( fp, (long)recno * recsize, SEEK_SET ) ) {
		return ( SEEK_ERROR );
	}
	if ( fread( buf, recsize, 1, fp ) == 0 && feof( fp ) ) {
		return ( SEEK_AFTER_EOF );
	}

	if ( buf[recsize-2] != '\r' || buf[recsize-1] != '\n' ) {
		return ( BAD_RECORD );
	}
	if ( ctrl_sw.ent_obj == COMET_IN && buf[59] != '.' ) {
		return ( BAD_RECORD );
	}
	if ( ctrl_sw.ent_obj != COMET_IN && buf[27] != '.' ) {
		return ( BAD_RECORD );
	}

	return ( 0 );
}


static int	get_rec_size(FILE *fp)
{
	register int	i,	type;
	static int	recsizes[][2] = {
		{ COMET_SHORT,	MPLA_SHORT },
		{ COMET_LONG,	MPLA_LONG },
		{ COMET_LONG,	MPLA_IAU },
		{ COMET_LONG,	MPLA_IAU_EXT },
	};			/* only last one is available to write */
	int	retval = -1;

	type = ( ctrl_sw.ent_obj == COMET_IN ) ? 0 : 1;
	for ( i = 0; i < sizeof(recsizes)/sizeof(recsizes[0]); i++ ) {
		retval = recsizes[i][type];
		if ( seek_and_load( fp, 0, retval, read_buf ) == 0 )
			return ( retval );
	}

	return ( ( global_sw.exec_mode == EXEC_FILER ) ? retval : -1 );
}


FILE	*eph_fopen( recsize )
int	*recsize;
{
	FILE	*fp;
	char	fname[FN_LEN+1];
	char	*mode;

	if ( ctrl_sw.ent_obj == COMET_IN ) {
		if ( global_sw.exec_mode == EXEC_EPH
		  || global_sw.exec_mode == EXEC_FILER ) {
			strcpy(fname, current.w_orb_comet);
		} else {
			strcpy(fname, current.comet_file);
		}
	} else {
		if ( global_sw.exec_mode == EXEC_EPH
		  || global_sw.exec_mode == EXEC_FILER ) {
			strcpy(fname, current.w_orb_mpla);
		} else {
			strcpy(fname, current.mpla_file);
		}
	}
	clip_blanks( fname );
	mode = ( global_sw.exec_mode == EXEC_FILER ) ? "r+b" : "rb";
	fp = fopen( fname, mode );
	if ( fp == NULL && global_sw.exec_mode == EXEC_FILER ) {
		mode = "w+b";
		fp = fopen( fname, mode );
	}
	*recsize = -1;
	if ( fp != NULL ) {
		*recsize = get_rec_size( fp );
	}

	return ( fp );
}

/*
 *	last_rec ---
 *		return last rec No.
 *		-1 means NO record in the file.
 *		-99 means bad file length.
 */
static int	last_rec(FILE *fp, int recsize)
{
	int		recno;
	long	sp,	mod;
	char	buf[10];

	fseek( fp, 0L, SEEK_END );
	sp = ftell( fp );
	mod = LMOD( sp, (long)recsize );
	if ( mod == 0L ) {
		recno = (int)( ( sp + 1 ) / (long)recsize ) - 1;
	} else {
		recno = -99;
#if defined(_WIN32)				/* accept only if the last 1 byte is ^Z */
		if ( mod == 1L ) {
			fseek( fp, -1L, SEEK_END );
			fread( buf, 1, 1, fp );
			if ( buf[0] == ctrl('Z') ) {
				sp--;
				recno = (int)( ( sp + 1 ) / (long)recsize ) - 1;
			}
		}
#endif
		if ( recno < -1 )
			warning( "BAD file length !!" );
	}

	return ( recno );
}

/*
 *	cat_item ---
 *		set item to write buffer
 */
static int	cat_item( char *fmt, ... )
{
	int		i;
	char		buf[500];
	va_list		arg_ptr;

	va_start( arg_ptr, fmt );
	i = vsprintf( buf, fmt, arg_ptr );
	strcat( read_buf, buf );			/* looks strange ? */
	va_end( arg_ptr );

	return ( i );
}

/*
 *	level 1
 */
static double	get_jd(int y, int m, double d)
{
	Time	t;
	double	ival;

	t.year = y;
	t.mon  = m;
	d = modf( d, &ival ) * 24.0;		t.day  = (int)ival;
	d = modf( d, &ival ) * 60.0;		t.hour = (int)ival; 
	d = modf( d, &ival ) * 60.0 * 100.0;	t.min  = (int)ival;
	t.csec = (int)d;

	return ( day2jd( t ) );
}


static int	get_el_comet(char *buf, int recsize, Orbit_element *el_buf, Comment *cm_buf)
{
	int	year,	month;
	double	day;
	char	wk[20];
	char	*wk1;
	double	wk5;

	memset( (char *)el_buf, '\0', sizeof( Orbit_element ) );
	memset( (char *)cm_buf, '\0', sizeof( Comment ) );

 	strcpy( el_buf->roman,  mid( buf,   0, 11 ) );
	strcpy( el_buf->number, mid( buf, 188, 10 ) );
	strcpy( el_buf->name,   mid( buf, 138, 28 ) );

	strcpy( wk, mid( buf, 11, 4 ) );
	year = atoi( wk );
	month = strmon2mon( mid( buf, 16, 5 ) );
	day = atof( mid( buf, 21, 8 ) );
	el_buf->time_pass = get_jd( year, month, day );
	el_buf->epoch = el_buf->time_pass;
	if ( buf[86] != ' ' ) {
		strcpy( wk, mid( buf, 86, 4 ) );
		year  = atoi( wk );
		month = strmon2mon( mid( buf, 91, 5 ) );
		day   = atof( mid( buf, 96, 4 ) );
		el_buf->epoch   = get_jd( year, month, day );
	}
	el_buf->peri_dis    = atof( mid( buf,  29, 11 ) );
	el_buf->ecc         = atof( mid( buf,  40,  9 ) );
	el_buf->peri        = DEG2RAD( atof( mid( buf,  56, 10 ) ) );
	el_buf->node        = DEG2RAD( atof( mid( buf,  66, 10 ) ) );
	el_buf->incl        = DEG2RAD( atof( mid( buf,  76, 10 ) ) );
	if ( el_buf->ecc < 1.0 ) {
		el_buf->sm_axis      = el_buf->peri_dis / ( 1.0 - el_buf->ecc );
		el_buf->period       = el_buf->sm_axis   * sqrt( el_buf->sm_axis );
		el_buf->daily_motion = DEG2RAD( 0.985607669 / el_buf->sm_axis
													/ sqrt(el_buf->sm_axis) );
	} else if ( el_buf->ecc > 1.0 ) {
		el_buf->sm_axis      = el_buf->peri_dis / ( 1.0 - el_buf->ecc );
	}
	cm_buf->n_obs         = atoi( mid( buf, 101, 6 ) );
	cm_buf->n_perterb_pla = atoi( mid( buf, 132, 2 ) );
	strcpy( cm_buf->arc,  mid( buf, 109, 23 ) );

	el_buf->h1 = 0.0;
	if ( recsize == COMET_LONG ) {
		el_buf->h1 = atof( mid( buf, 167, 5 ) );
		el_buf->k1 = atof( mid( buf, 172, 5 ) );
		el_buf->h2 = atof( mid( buf, 177, 5 ) );
		el_buf->k2 = atof( mid( buf, 182, 5 ) );
		strcpy( cm_buf->note, mid( buf, 199, 56 ) );
		ctrl_sw.comment_sw = 1;			/* display comment */
	}
	if ( el_buf->h1 == 0.0 ) {
		el_buf->h1 = 10.0;
		el_buf->k1 = 10.0;
		el_buf->h2 = 15.0;
		el_buf->k2 =  5.0;
	}
	el_buf->tail = 0.020;

	return ( 0 );
}


static int	set_buf_comet(char *recbuf, Orbit_element *el_buf, Comment *cm_buf)
{
	int	len = 0;
	double	day;
	Time	tm;

	read_buf[0] = '\0';
	len += cat_item( "%-11s", mid( el_buf->roman, 0, 11 ) );
	day = jd2day( el_buf->time_pass, &tm );
	len += cat_item( "%4d", tm.year );
	len += cat_item( " %-5s", mon2strmon( tm.mon ) );
	len += cat_item( "%8.5lf", day );

	len += cat_item( " " );
	len += cat_item( "%-10.7lf", el_buf->peri_dis );
	if ( el_buf->ecc >= 0.0 ) {
		len += cat_item( "%9.7lf", el_buf->ecc );
	} else {
		len += cat_item( "%+8.6lf", el_buf->ecc );
	}
	if ( el_buf->ecc < 1.0 && el_buf->period < 1000.0 ) {
		len += cat_item( "%6.2lf",  el_buf->period );
	} else {
		len += cat_item( "      " );
	}
	len += cat_item( "%10.5lf",  RAD2DEG( el_buf->peri ) );
	len += cat_item( "%10.5lf",  RAD2DEG( el_buf->node ) );
	len += cat_item( "%10.5lf",  RAD2DEG( el_buf->incl ) );

	day = jd2day( el_buf->epoch, &tm );
	len += cat_item( "%5d", tm.year );
	len += cat_item( " %5s", mon2strmon( tm.mon ) );
	len += cat_item( "%4.1lf", day );

	len += cat_item( "%6d", cm_buf->n_obs );
	len += cat_item( " " );
	len += cat_item( "%-23.23s", cm_buf->arc );
	len += cat_item( "%4d", cm_buf->n_perterb_pla );
	len += cat_item( " " );
	len += cat_item( "   " );
	len += cat_item( "%-28.28s", el_buf->name );
	len += cat_item( " " );		/* delete flag */
	len += cat_item( "%5.2lf", el_buf->h1 );
	len += cat_item( "%5.2lf", el_buf->k1 );
	len += cat_item( "%5.2lf", el_buf->h2 );
	len += cat_item( "%5.2lf", el_buf->k2 );
	len += cat_item( " " );
	len += cat_item( "%-10.10s", el_buf->number );
	len += cat_item( " %-56.56s", cm_buf->note );
	len += cat_item( "\r\n" );

	if ( len == COMET_LONG )
		strncpy( recbuf, read_buf, len );
	return ( len == COMET_LONG ? 0 : -1 );
}


static int	get_el_mpla(char *buf, int recsize, Orbit_element *el_buf, Comment *cm_buf)
{
	char	wk[20];
	double	wk5;

	memset( (char *)el_buf, '\0', sizeof( Orbit_element ) );
	memset( (char *)cm_buf, '\0', sizeof( Comment ) );

	if ( recsize == MPLA_SHORT || recsize == MPLA_LONG ) {
		strcpy( el_buf->number, mid( buf,  0,  6 ) );
		strcpy( el_buf->name,   mid( buf,  6, 16 ) );
		el_buf->epoch        = atof( mid( buf, 22, 10 ) );
		el_buf->h2           = atof( mid( buf, 33,  6 ) );
		el_buf->g            = atof( mid( buf, 39,  5 ) );
		el_buf->mean_anomaly = DEG2RAD( atof( mid( buf, 44, 10 ) ) );
		el_buf->peri         = DEG2RAD( atof( mid( buf, 54, 10 ) ) );
		el_buf->node         = DEG2RAD( atof( mid( buf, 64, 10 ) ) );
		el_buf->incl         = DEG2RAD( atof( mid( buf, 74, 10 ) ) );
		el_buf->ecc          = atof( mid( buf, 84, 10 ) );
		el_buf->daily_motion = DEG2RAD( atof( mid( buf, 94, 11 ) ) );
		el_buf->sm_axis      = atof( mid( buf, 105, 11 ) );

		if ( el_buf->daily_motion == 0.0 ) {
			el_buf->daily_motion = DEG2RAD( 0.985607669 / el_buf->sm_axis
													/ sqrt(el_buf->sm_axis) );
		};
		el_buf->period       = el_buf->sm_axis * sqrt( el_buf->sm_axis );
		el_buf->time_pass    = el_buf->epoch - el_buf->mean_anomaly
										  / el_buf->daily_motion;
		el_buf->peri_dis     = el_buf->sm_axis * ( 1.0 - el_buf->ecc );
		if ( recsize == MPLA_LONG ) {
			strcpy( cm_buf->note,
					mid( buf, MPLA_SHORT-1, ( MPLA_LONG - MPLA_SHORT )-1 ) );
			ctrl_sw.comment_sw = 1;			/* display comment */
		}
	} else {			/* IAU compatible */
		Time	t;

		if ( buf[5] == ' ' )
			strcpy( el_buf->number, mid( buf,  0,  5 ) );
		else
			strcpy( el_buf->number, mid( buf,  0,  7 ) );

		el_buf->h2           =          atof( mid( buf,   7,  7 ) );
		el_buf->mean_anomaly = DEG2RAD( atof( mid( buf,  14,  9 ) ) );
		el_buf->peri         = DEG2RAD( atof( mid( buf,  23, 10 ) ) );
		el_buf->node         = DEG2RAD( atof( mid( buf,  33, 10 ) ) );
		el_buf->incl         = DEG2RAD( atof( mid( buf,  43,  9 ) ) );
		el_buf->ecc          =          atof( mid( buf,  52, 10 ) );
		el_buf->daily_motion = DEG2RAD( atof( mid( buf,  62, 11 ) ) );
		el_buf->sm_axis      =          atof( mid( buf,  73, 11 ) );
		el_buf->g            =          atof( mid( buf,  84,  6 ) );
		t.year = atoi( mid( buf, 90, 4 ) );
		t.mon  = atoi( mid( buf, 94, 2 ) );
		t.day  = atoi( mid( buf, 96, 2 ) );
		t.hour = t.min = t.csec = 0;
		el_buf->epoch        = day2jd( t );
 
		if ( el_buf->daily_motion == 0.0 ) {
			el_buf->daily_motion = DEG2RAD( 0.985607669 / el_buf->sm_axis
													/ sqrt(el_buf->sm_axis) );
		};
		el_buf->period       = el_buf->sm_axis * sqrt( el_buf->sm_axis );
		el_buf->time_pass    = el_buf->epoch - el_buf->mean_anomaly
										  / el_buf->daily_motion;
		el_buf->peri_dis     = el_buf->sm_axis * ( 1.0 - el_buf->ecc );
		if ( recsize == MPLA_IAU_EXT ) {
			strcpy( el_buf->name, mid( buf, 111, 30 ) );
			strcpy( cm_buf->note, mid( buf, 141, 53 ) );
			ctrl_sw.comment_sw = 1;			/* display comment */
		}
	}

	return ( 0 );
}


static int	set_buf_mpla(char *recbuf, Orbit_element *el_buf, Comment *cm_buf)
{
	int	len = 0;
	Time	t;

	read_buf[0] = '\0';
	jd2day( el_buf->epoch, &t );
	if ( 1900 <= t.year && t.year <= 1999 )
		t.year -= 1900;

	if ( el_buf->number[5] == ' ' ) {
		len += cat_item( "%05d  ", atoi(el_buf->number) );
	} else {
		len += cat_item( "%07d",  atoi(el_buf->number) );
	}
	len += cat_item( "%7.2lf",   el_buf->h2 );
	len += cat_item( "%9.5lf",   RAD2DEG(el_buf->mean_anomaly) );
	len += cat_item( "%10.5lf",  RAD2DEG(el_buf->peri) );
	len += cat_item( "%10.5lf",  RAD2DEG(el_buf->node) );
	len += cat_item( "%9.5lf",   RAD2DEG(el_buf->incl) );
	if ( el_buf->ecc >= 0.0 ) {
		len += cat_item( "%10.7lf",  el_buf->ecc );
	} else {
		len += cat_item( "%+9.6lf",  el_buf->ecc );
	}
	len += cat_item( "%11.8lf",  RAD2DEG(el_buf->daily_motion) );
	len += cat_item( "%11.7lf",   el_buf->sm_axis );
	len += cat_item( "%6.2lf",  el_buf->g );
	len += cat_item( "%4d%02d%02d", t.year, t.mon, t.day );
	len += cat_item( "          " );	/* Reference */

	len += cat_item( "  " );		/* delimiter(CR/LF) */
	len += cat_item( " " );			/* delete flag */
	len += cat_item( "%-30.30s", el_buf->name );
	len += cat_item( "%-53.53s", cm_buf->note );
	len += cat_item( "\r\n" );

	if ( len == MPLA_IAU_EXT )
		strncpy( recbuf, read_buf, len );

	return ( len == MPLA_IAU_EXT ? 0 : -1 );
}


/*
 *	load_element ---
 *		load orbital element from file
 */
int
load_element( fp, recno, recsize, el_buf, cm_buf )
FILE		*fp;
int		recno;
int		recsize;
Orbit_element	*el_buf;
Comment		*cm_buf;
{
	int		retval;

	retval = seek_and_load( fp, recno, recsize, read_buf );
	if ( retval == 0 ) {
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			retval = get_el_comet( read_buf, recsize, el_buf, cm_buf );
		} else {
			retval = get_el_mpla( read_buf, recsize, el_buf, cm_buf );
		}
	}

	return ( retval );
}


static MenuFunc	save_element()
{
	int		x=20,	y=Y_BIAS+13;
	int		recsize,	recno;
	long		sp;
	FILE		*fp;
	Time		tm;
	double		day;
	Fname		fname;
	int		retval;


	if ( global_sw.el_buf_stat == EL_NOT_READY ) {
		warning( "Element NOT ready!" );
		guidance( GUID_SUB );
		return ( 0 );
	}
	if ( global_sw.el_buf_stat == EL_SAVED ) {
		retval = warning( "Already saved.Append more?(y/n)" );
		if ( retval != 'y' || retval != 'Y' ) {
			guidance( GUID_SUB );
			return ( 0 );
		}
	}
	frame_box(x-2, y-1, 47, 3);

	fname.xpos   = x;
	fname.ypos   = y-Y_BIAS;
	fname.flg	= 1;			/* create */
	if ( ctrl_sw.ent_obj == COMET_IN ) {
		fname.varptr = current.w_orb_comet;
	} else {
		fname.varptr = current.w_orb_mpla;
	}

	moveto( x, y );
	dos_puts( fname.varptr );
	retval = get_fname( &fname );
	if ( ! ( retval & UPPER_MENU ) ) {
		retval = UPPER_MENU;
		if ( ( fp=eph_fopen( &recsize ) ) == NULL ) {
			warning( "Output file open error!!" );
		} else {
			if ( ( ctrl_sw.ent_obj == COMET_IN    && recsize != COMET_LONG )
			  || ( ctrl_sw.ent_obj == ASTEROID_IN && recsize != MPLA_IAU_EXT ) ) {
				warning( "Not work file!" );
			} else {
				if ( ctrl_sw.ent_obj == COMET_IN ) {
					retval = set_buf_comet( read_buf, &orb_el_buf, &comment );
				} else {
					retval = set_buf_mpla( read_buf, &orb_el_buf, &comment );
				}
				if ( retval ) {
					warning( "Value set error !!" );
				} else {
					if ( ( recno = last_rec( fp, recsize ) ) < -1 ) {
						warning( "BAD file length !!" );
					} else {
						sp  = (long)recno + 1L;
						sp *= (long)recsize;
						fseek( fp, sp, SEEK_SET );
						retval = fwrite( read_buf, recsize, 1, fp );
						if ( retval ) {
							global_sw.el_buf_stat = EL_SAVED;
						} else {
							warning( "Write error !!" );
						}
					}
				}
			}
			fclose( fp );
		}
	} else {
		retval = 0;
	}
	clear_bottom( x-2, y-1 );

	return ( retval );
}


/*
 *	mask function
 */
/*
 *	is_live ---
 *		Is the record alive ?
 */
int
is_live( dmy )
Orbit_element	dmy;
{
	int		retval = FALSE;

	if ( ctrl_sw.ent_obj == COMET_IN ) {
		if ( read_buf[ COMET_SHORT-2 ] == '\r' ) {
			retval = TRUE;
		} else if ( read_buf[ COMET_LONG-2 ] == '\r' ) {
			if ( read_buf[ COMET_SHORT-2 ] == ' ' )
				retval = TRUE;
		}
	} else {
		if ( read_buf[ MPLA_SHORT-2 ] == '\r'
		  || read_buf[ MPLA_IAU  -2 ] == '\r' ) {
			retval = TRUE;
		} else if ( read_buf[ MPLA_LONG-2 ] == '\r' ) {
			if ( read_buf[ MPLA_SHORT-2 ] == ' ' )
				retval = TRUE;
		} else if ( read_buf[ MPLA_IAU_EXT-2 ] == '\r' ) {
			if ( read_buf[ MPLA_IAU-2 ] == ' ' )
				retval = TRUE;
		}
	}

	return ( retval );
}

/*
 *	is_deleted ---
 *		Is the record deleted ?
 */
int
is_deleted( dmy )
Orbit_element	dmy;
{
	return ( ( is_live( dmy ) ? FALSE : TRUE ) );
}


/*
 *	display function
 */
int
display_element( n, recsize, el_buf, cm_buf )
int		n,	recsize;
Orbit_element	el_buf;
Comment		cm_buf;
{
	int	save;

	clear_bottom( 0, Y_BIAS );
	moveto( 0, Y_BIAS+2 );
	dos_printf( "          Rec.No:%-4d\n\n", n+1 );

	if ( recsize == COMET_LONG
	  || recsize == MPLA_LONG
	  || recsize == MPLA_IAU_EXT ) {
		save = ctrl_sw.comment_sw;		/* save old */
/*		ctrl_sw.comment_sw = 1; */		/* force display comment */
/*		ctrl_sw.comment_sw = 0; */
	}
	orb_el_out( &el_buf, &cm_buf, stderr );
	if ( recsize == COMET_LONG
	  || recsize == MPLA_LONG
	  || recsize == MPLA_IAU_EXT ) {
		ctrl_sw.comment_sw = save;
	}

	return ( 0 );
}

/*
 *	when CR pressed
 */
int
copy_to_calc_buf( dmy0, dmy1, dmy2, el_buf, cm_buf )
FILE		*dmy0;
int		dmy1;
int		dmy2;
Orbit_element	el_buf;
Comment		cm_buf;
{
	if (  global_sw.el_buf_stat == EL_READY
	  && warning( "Break element buffer ? (y/n)" ) == 'n' ) {
			return ( FALSE );
	}

	orb_el_buf = el_buf;
	comment	= cm_buf;	/* save even if not necesarry to calculation */
	global_sw.el_buf_stat = EL_READY;
	arry_max = 0;		/* force recalculate */

	return ( TRUE );
}


static MenuFunc	edit_comment()
{
	int		x,	y=Y_BIAS+12;
	int		ik,	len;

	len = ( ctrl_sw.ent_obj == COMET_IN ) ? 74 : 53;
	x = ( EPH_cols - len ) / 2;

	clear_bottom( x-2, y-1 );
	frame_box(x-2, y-1, len+3, 3);
	moveto( x, y );
	dos_puts( comment.note );

	ik = fmt_gets( x, y, NULL, comment.note, len );		/* free format */

	clear_bottom( x-2, y-1 );

	return ( ( ik == ESCAPE ) ? 0 : 1 );
}



static int	set_delflg(int recsize, char flg)
{
	int		retval = -1;

	if ( ctrl_sw.ent_obj == COMET_IN ) {
		if ( recsize == COMET_LONG ) {
			read_buf[ COMET_SHORT-2 ] = flg;
			retval = 0;
		}
	} else {
		switch ( recsize ) {
		case MPLA_LONG:
				read_buf[ MPLA_SHORT-2 ] = flg;
				retval = 0;
				break;
		case MPLA_IAU_EXT:
				read_buf[ MPLA_IAU-2 ] = flg;
				retval = 0;
				break;
		}
	} 

	return ( retval );
}



static int	del_elem( fp, recno, recsize, dmy1, dmy2 )
FILE		*fp;
int		recno, recsize;
Orbit_element	dmy1;
Comment		dmy2;
{
	int		retval = 0;

	if ( set_delflg( recsize, '*' ) ) {
		warning( "Can't delete Library!!" );
	} else {
		fseek( fp, (long)recno * (long)recsize, SEEK_SET );
		retval = fwrite( read_buf, recsize, 1, fp );
		if ( retval == 0 ) {
			warning( "Delete error !!" );
		}
	}

	return ( retval );
}

static int	undel_elem( fp, recno, recsize, dmy1, dmy2 )
FILE		*fp;
int		recno, recsize;
Orbit_element	dmy1;
Comment		dmy2;
{
	int	retval = 0;

	if ( set_delflg( recsize, ' ' ) ) {
		warning( "Can't Un-delete Library!!" );
	} else {
		fseek( fp, (long)recno * (long)recsize, SEEK_SET );
		retval = fwrite( read_buf, recsize, 1, fp );
		if ( retval == 0 ) {
			warning( "Delete error !!" );
		}
	}

	return ( retval );
}


/*
 *	scan_file ---
 *		Scan file and load it
 */
MenuFunc
scan_file( cmd )
Filer_cmd	*cmd;
{
	register int	recno,	done;
	int		recsize,	err = FALSE;
	int		sk,	fore_rec;
	int		found,	direction;
	int		ik;
	FILE		*fp;
	Orbit_element	w_el_buf;
	Comment		w_cm_buf;
	MenuFunc	get_keystr();

	top_bar( cmd->level_code );

	if ( ( fp=eph_fopen( &recsize ) ) == NULL ) {
		pause( "Data file open error!!" );
		err = TRUE;
	} else if ( recsize < 0 ) {
		warning( "Bad file record" );
		err = TRUE;
	} else if ( get_keystr( recsize ) ) {
		ik = ESCAPE;
		err = TRUE;
	}

	if ( ! err ) {
		clear_bottom( 0, Y_BIAS );
		done = FALSE;
		if ( cmd->level_code == TB_LOAD_ELEMENT
		  && ctrl_sw.scan_dir ) {
			fore_rec  =  recno = last_rec( fp, recsize ) + 1;
			direction = -1;		/* read backward */
		} else {
			fore_rec  =  recno = -1;
			direction =  1;		/* read forward */
		}
		do {
			found = 0;
			guidance( GUID_SELECT_F );
			if ( ( recno += direction ) < 0 )
				sk = SEEK_BEFORE_TOP;
			else
				sk = load_element( fp, recno, recsize, &w_el_buf, &w_cm_buf );

			while ( sk == 0 ) {
				int w;

				w = MatchKey( &w_el_buf, &w_cm_buf );
				if ( w == TRUE ) {
					found = TRUE;
					/* *****  call masking function ***** */
					if ( cmd->mask_func != NULL ) {
						found = ( *cmd->mask_func )( w_el_buf );
					}
					if ( found )
						break;
				}
				if ( ( recno += direction ) < 0 )
					sk = SEEK_BEFORE_TOP;
				else
					sk = load_element( fp, recno, recsize,
											&w_el_buf, &w_cm_buf );
			}

			if ( found ) {
				fore_rec = recno;

				clear_keybuf();

				/* *****  call display function ***** */
				if ( cmd->disp_func != NULL ) {
					( *cmd->disp_func )( recno, recsize, w_el_buf, w_cm_buf );
				}

loop:
				ik = getchr();
				if ( isupper(ik) )
					ik = tolower(ik);

				switch ( ik ) {
				case CR:
						/* *****  call function when CR pressed  ***** */
						if ( cmd->when_CR != NULL ) {
							done = ( *cmd->when_CR )( fp, recno, recsize,
													  w_el_buf, w_cm_buf );
						}
						break;
				case ctrl('C'):
						ik = ESCAPE;
						/* No break */
				case ESCAPE:
						done = TRUE;
						break;
				case 'p':
				case BS:
				case LEFT_ARROW:
						direction = -1;
						break;
				case 'n':
				case ' ':
				case RIGHT_ARROW:
						direction =  1;
						break;
				default:
						RingBell();
						goto loop;
				}
			} else {
				switch ( sk ) {
				case SEEK_BEFORE_TOP:
						ik = warning("Exceed top of file[ESC:exit]");
						if (ik == ESCAPE || ik == 'n') {
							done = TRUE;
						} else {
							recno = fore_rec + 1;
						}
						break;
				case SEEK_AFTER_EOF:
						ik = warning("Exceed end of file[ESC:exit]");
						if (ik == ESCAPE || ik == 'n') {
							done = TRUE;
						} else {
							recno = fore_rec - 1;
						}
						break;
				case BAD_RECORD:
						warning( "Bad file record" );
						done = TRUE;
						ik = ! ESCAPE;
				default:
						goto maybe_error;
				}
			}
		} while ( ! done );
maybe_error:
		clear_bottom( 0, Y_BIAS );
	}
	if (fp)
		fclose( fp );

	guidance( GUID_SUB );

	return ( ( ik == ESCAPE ? 1 : ( ik == CR ) ? 0 : menu_upper() ) );
}


#if defined(_WIN32)

static MenuFunc	pack_file()
{
	register int	r_rec,	w_rec;
	int		x=20,	y=15;
	int		sk,	recsize;
	FILE		*fp;
	Orbit_element	dmy;

	if ( warning( "Pack file.Sure ?(y/n)" ) == 'n' ) {
		sk = 0;
	} else {
		clear_bottom( x, y );
		reverse_start();
		dos_puts( "Checking File structure ...." );
		reverse_end();

		fp = eph_fopen( &recsize );
		if ( fp == NULL || recsize < 0 ) {
			pause( "Data file open error!!" );
			if (fp)
				fclose(fp);

			return ( menu_upper() );
		}
		r_rec = w_rec = 0; 
		sk = seek_and_load( fp, r_rec, recsize, read_buf );
		while ( sk == 0 ) {
			if ( is_deleted( dmy ) )	{
				w_rec++;		/* use for delete counter temporarily */
			}
			r_rec++;
			sk = seek_and_load( fp, r_rec, recsize, read_buf );
		}
		clear_bottom( x, y );
		if ( ! ( sk == SEEK_AFTER_EOF && w_rec ) ) {
			if ( w_rec == 0 )
				warning( "No deleted record." );
			else
				warning( "Bad file structure. Sorry." );
		} else {
			moveto( x, y );
			reverse_start();
			dos_puts( "Now pack start. Wait a moments ..." );
			reverse_end();

			r_rec = w_rec = 0; 
			sk = seek_and_load( fp, r_rec, recsize, read_buf );
			while ( sk == 0 ) {
				if ( is_live( dmy ) ) {
					if ( r_rec != w_rec ) {
						fseek( fp, (long)w_rec*(long)recsize, SEEK_SET );
						fwrite( read_buf, recsize, 1, fp );
					}
					w_rec++;
				}
				r_rec++;
				sk = seek_and_load( fp, r_rec, recsize, read_buf );
			}
			if ( chsize( fileno( fp ), (long)w_rec*(long)recsize ) ) {
				warning( "File trunc error!!" );
			}
			fclose( fp );
		}
	}
	guidance( GUID_SUB );
	clear_bottom( x, y );

	return ( sk == 0 ? 0 : menu_upper() );
}
#endif /* _WIN32 */


/*
 *	menu item
 */
static Filer_cmd	filer_cmd[] = {
	{ TB_FILER_DELETE,		is_live,	display_element, del_elem	},
	{ TB_FILER_UNDELETE,	is_deleted,	display_element, undel_elem  },
	{ TB_LOAD_ELEMENT,		is_live,	display_element, copy_to_calc_buf },
} ;

extern MenuFunc	ent_orbit();

static Menu	filer_menu[] = {
	{ WAIT_KEY,	'L',	"Load Orbital Element",	scan_file,	&(filer_cmd[2])  },
	{ WAIT_KEY,	'E',	"Edit Orbital Element",	ent_orbit,	NULL  },
	{ WAIT_KEY,	'C',	"Edit Comment field  ",	edit_comment,	NULL  },
	{ WAIT_KEY,	'S',	"Save   Orbit element",	save_element,	NULL  },
	{ WAIT_KEY,	'D',	"Delete Orbit element",	scan_file,	&(filer_cmd[0])  },
	{ WAIT_KEY,	'U',	"Undel  Orbit element",	scan_file,	&(filer_cmd[1])  },
#if defined(_WIN32)
	{ WAIT_KEY,	'P',	"Pack   Orbit file   ",	pack_file,	NULL  },
#endif
	{ WAIT_KEY,	'I',	"Import MPC Orbit    ",	file_sel,	NULL  },
	{ WAIT_KEY,	'B',	"Back to Main Menu   ",	menu_upper,	NULL  },
	{ NOT_EXEC,	'\0',	NULL,					NULL,			NULL  },
} ;


/*
 *	filer ---
 *		filer menu
 */
int
filer()
{
	int		w_mode;
	int		deflt,	result;

	w_mode = global_sw.exec_mode;
	global_sw.exec_mode = EXEC_FILER;
	clear_bottom( 25, Y_BIAS+5 );

	deflt = 0;			/* load orbital elements */
	do {
		top_bar( TB_FILER_SUB );
		guidance( GUID_SUB );
		result = menu_man( filer_menu, 25, 2+Y_BIAS, MN_SELECT, deflt );
		deflt = ITEM_NO( result );
	} while ( ! ( result & UPPER_MENU || result & QUIT ) );
	global_sw.exec_mode = w_mode;

	return ( menu_upper() );
}

/* **************  end of filer.c  ************* */
