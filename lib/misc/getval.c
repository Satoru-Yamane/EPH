/*
 *	getval.c
 *		Get/Put value routine for EPH
 *
 *	(C)opyright 1990  all right reserved
 *		Dec.25, 1990	by S.Yamane
 *		Feb.19, 1992	by K.Osada
 *
 *	$Header: GETVAL.Cv  1.2  91/01/08 15:25:22  yamane  Rel $
 */

#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#if defined(_WIN32) || __FreeBSD__ || __linux__
#	include	<string.h>
#else
#	include	<strings.h>
#endif
#include	<stdlib.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"getval.h"
#include	"ephcurs.h"

/*
 *	functions
 */
static char	*get_v3(char *str, int *i1, int *i2, double *f1, int *sign)
{
	*sign = 1;
	*i1 = *i2 = 0;
	*f1 = 0.0;
	while ( *str == ' ' )
		str++;

	if (*str == '-') {
		*sign = -1;
		str++;
	} else if (*str == '+') {
		*sign = 1;
		str++;
	}
	while ( isdigit( *str ) ) {
		*i1 = *i1 * 10 + ( *str - '0' );
		str++;
	}
	while ( *str && ! isdigit( *str ) )
		str++;

	while ( isdigit( *str ) ) {
		*i2 = *i2 * 10 + ( *str - '0' );
		str++;
	}
	while ( *str && ! isdigit( *str ) )
		str++;

	if ( *str ) {
		*f1 = atof(str);
		while ( isdigit( *str ) || *str == '.' )
			str++;
	}

	return ( str );
}

/*
 *	str2val ---
 *		string to value (numeric)
 */
int
str2val( str, vartype, varptr )
char	*str;
int	vartype;
void	*varptr;
{
	double w_ut2et;

	if ( vartype == VAL_STRING ) {
		strcpy( (char *)varptr, str );
	} else {
		while ( *str == ' ' )
			str++;

		switch ( vartype ) {
		case VAL_INTEGER:
				*(int *)varptr = atoi(str);
				break;
		case VAL_DOUBLE:
		case VAL_DEGREE:
			{
				double	w;

				w = atof(str);
				if ( vartype == VAL_DEGREE )
					w = DEG2RAD( w );
				*(double *)varptr = w;
				break;
			}
		case VAL_LONG_JD:
		case VAL_SHORT_JD:
			{
				int		y,	m,	ho,	mi,	sign;
				double	day,	sec;
				Time	tm;
				double	dmy,	ival;
				double	jde;
			
				str = get_v3( str, &y, &m, &day, &sign );
				tm.year = y * sign;
				tm.mon  = m;
				tm.day  = (int)day;
				if ( vartype == VAL_LONG_JD ) {
					str = get_v3( str, &ho, &mi, &sec, &sign );
					tm.hour = ho;
					tm.min  = mi;
					tm.csec = (int)(sec * 100.0 + 0.5 );
				} else {
					tm.day = day2time( day, &tm.hour, &tm.min, &tm.csec );
				}
				jde = day2jd( tm );
				switch ( ctrl_sw.time_system ) {
				case JST_IN:
					jde = JST2UT( jde );
					/* No break */
				case UT_IN:
					w_ut2et = ut2et(jde);
					if(w_ut2et == 0) {
						jde += current.ut_et;
						} else {
						jde += ( ctrl_sw.ut2et_sw ? current.ut_et : w_ut2et );
						}
					/* No break */
				case ET_IN:	case TT_IN:
					break;
				}
			
				*(double *)varptr = jde;
				break;
			}
		case VAL_TIME:
			{
				int		ih,	im,	is,	sign;
				double	s;
			
				str = get_v3( str, &ih, &im, &s, &sign );
				is = (int)s;
			
				*(double *)varptr = time2day( ih, im, is*100 );
				*(double *)varptr *= (double)sign;
				break;
			}
		case VAL_RA:
			{
				int		ra_h,	ra_m,	sign;
				double	ra_s;
			
				get_v3( str, &ra_h, &ra_m, &ra_s, &sign );
			
				*(double *)varptr = hms2ra( ra_h, ra_m, ra_s );
				*(double *)varptr *= (double)sign;
				break;
			}
		case VAL_DECL:
			{
				int		d,	m,	sign;
				double	s;
			
				get_v3( str, &d, &m, &s, &sign );
				*(double *)varptr = dms2rad( d*sign, m, s );
/*				*(double *)varptr *= (double)sign;	*/
				break;
			}
		default:
				return ( FALSE );
		}
	}

	return ( TRUE );
}


/*
 *	valsprintf ---
 *		sprintf : value version
 */
int valsprintf(char *buf, char *fmt, int val_type, void *valp)
{
	Time	T;
	double	w, w_et2ut;

	w = *(double *)valp;
	switch ( val_type ) {
	case VAL_STRING:
			if ( fmt )
				sprintf( buf, fmt, (char *)valp );
			else
				strcpy( buf, (char *)valp );
			break;
	case VAL_DEGREE:
			w = RAD2DEG( w );
			sprintf( buf, fmt, w );
			break;
	case VAL_INTEGER:
			sprintf( buf, fmt, *(int *)valp );
			break;
	case VAL_DOUBLE:
			sprintf( buf, fmt, w );
			break;
	case VAL_SHORT_JD:
	case VAL_LONG_JD:
			if ( ctrl_sw.time_system != ET_IN
			  && ctrl_sw.time_system != TT_IN )
				w_et2ut = et2ut( w );
				if(w_et2ut == 0) {
					w_et2ut = current.ut_et;
					}
				w -= ( ctrl_sw.ut2et_sw ? current.ut_et : w_et2ut );
			if ( ctrl_sw.time_system == JST_IN )
				w = UT2JST( w );
			w = jd2day( w, &T );

			if ( val_type == VAL_SHORT_JD )
				sprintf( buf, fmt, T.year, T.mon, w );
			else
				sprintf( buf, fmt,
						T.year, T.mon, T.day, T.hour, T.min, T.csec/100 );
			break;
	case VAL_TIME:
			{
				int	id,	ih,	im,	is;
			
				id = day2time( w, &ih, &im, &is );
				sprintf( buf, fmt, ih, im, is/100 );
			}
			break;
	case VAL_RA:
			{
				int		sign,	ra_h,	ra_m;
				double	ra_s;
			
				sign = ra2hms( w, &ra_h, &ra_m, &ra_s );
				if ( fabs(60.0-ra_s) < 0.005 ) {
					ra_s  = 0.0;
					if ( ++ra_m > 59 ) {
						ra_m = 0;
						++ra_h;
					}
				}
				sprintf( buf, fmt,  sign*ra_h, ra_m, ra_s );
			}
			break;
	case VAL_DECL:
			{
				register char	*p;
				int				sign,	d,	m;
				double			s;
			
				sign = rad2dms( w, &d, &m, &s );
				if ( fabs(60.0-s) < 0.05 ) {
					s  = 0.0;
					if ( ++m > 59 ) {
						m = 0;
						++d;
					}
				}
				sprintf( buf, fmt, sign*d, m, s );
			
				if ( sign < 0 ) {		/* for '-00' deg. */
					p = buf;
					while ( *p && ! isdigit( *p ) )
						p++;
					if ( *p && (--p > buf) ) {
						if ( *p == ' ' || *p == '+' )
							*p = '-';
					}
				}
			}
			break;
	default:
			return ( FALSE );
	}

	return ( TRUE );
}


/*
 *	putval ---
 *
 *	Jan.17, 1989   by S.Yamane
 */
char	*putval( param, dim )
Field	*param;
int	dim;
{
	register int	spc,	hyp;
	static char		wk[256];

	if ( param->hyphen == 0 ) {
		valsprintf( wk, param->disp_fmt, param->vartype, param->varptr );
	} else {
		wk[0] = '\0';
		spc = ( param->hyphen >> 10 ) & 0x1f;
		while ( spc-- )
			strcat( wk, " " );		/* head space */
		hyp = ( param->hyphen >>  5 ) & 0x1f;
		while ( hyp-- )
			strcat( wk, "-" );		/* hyphen */
		spc = ( param->hyphen	   ) & 0x1f;
		while ( spc-- )
			strcat( wk, " " );		/* trailer space */
	}

	moveto( param->x, param->y );
	if ( dim )
		halfbright_start();
	dos_puts( wk );
	if ( dim )
		halfbright_end();

	return ( wk );
}

/*
 *	getval ---
 *
 *	Jan.17, 1989   by S.Yamane
 */
MenuFunc	getval( param )
Field	*param;
{
	int		ik;
	int		retval;
	char	old_string[256],	edit_buf[256];

	strcpy( old_string, putval( param, 0 ) );		/* save old */

	strcpy( edit_buf, old_string );
	param->changed = FALSE;
	ik = fmt_gets( param->x, param->y,
					param->ed_fmt, edit_buf, param->field_len );
	if ( strcmp( old_string, edit_buf ) )
		param->changed = TRUE;

	str2val( edit_buf, param->vartype, param->varptr );

	if ( param->changed == TRUE )
		putval( param, 0 );

	if ( param->after_edit != NULL )
		(*param->after_edit)( param );

	switch ( ik ) {
	case ESCAPE:
			retval = menu_upper();	break;
	case ctrl('C'):
			retval = menu_quit();	break;
	case UP_ARROW:
			retval = FORE_ITEM;		break;
	default:
			retval = NEXT_ITEM;
	}

	return ( retval | NO_REWRITE );
}


#ifdef	USE_REGEX
RePat	*REcomp( pat )
char	*pat;
{
	int		ok = FALSE;
	RePat	*ret;

	ret = (RePat *)calloc( 1, sizeof( RePat ) );
	if ( ret != NULL ) {
		if ( ( ret->buffer = (char *)malloc( 200 ) ) != NULL ) {
			ret->allocated = 200;
			if ( ( ret->fastmap = (char *)malloc( 1 << BYTEWIDTH ) ) != NULL ) {
				if ( re_compile_pattern( pat, strlen( pat ), ret ) == NULL ) {
					ok = TRUE;
				}
				if ( ! ok )
					free( ret->fastmap );
			}
			if ( ! ok )
				free( ret->buffer );
		}
		if ( ! ok )
			free( ret );
	}

	return ( ok ? ret : NULL );
}

int	REfree( cpat )
RePat	*cpat;
{
	int	ret = FALSE;

	if ( cpat ) {
		free( cpat->fastmap );
		free( cpat->buffer );
		free( cpat );
		ret = TRUE;
	}

	return ( ret );
}

int	REmatch( cpat, str )
RePat	*cpat;
char	*str;
{
	int		len = strlen( str );

	return ( ( re_search( cpat, str, len, 0, len, 0 ) >= 0 ) ? TRUE : FALSE );
}

#endif	/* USE_REGEX */



/*
 *	valpcmp ---
 *		compare 2 values by pointer
 *			p1 >  p2   return  1
 *			p1 == p2   return  0
 *			p1 <  p2   return -1
 */
int
valpcmp( p1, p2, type )
void	*p1,	*p2;
int	type;
{
	int		ret = 0;
	int		l1,	l2,	l;
	double	fdiff;

	switch ( type ) {
	case VAL_STRING:
			l1 = strlen( (char *)p1 );
			l2 = strlen( (char *)p2 );
			l  = ( l2 > l1 ) ? l1 : l2;
			ret = strncmp( (char *)p1, (char *)p2, l );
			break;
	case VAL_INTEGER:
			ret = *(int *)p1 - *(int *)p2;
			break;
	case VAL_DEGREE:
	case VAL_RA:
			;
	case VAL_LONG_JD:
	case VAL_SHORT_JD:
	case VAL_TIME:
	case VAL_DECL:
	case VAL_DOUBLE:
			fdiff = *(double *)p1 - *(double *)p2;
			if ( fdiff > 0.0 )
				ret = 1;
			else if ( fdiff < 0.0 )
				ret = -1;
			break;
	}
	if ( ret > 0 )
		ret = 1;
	else if ( ret < 0 )
		ret = -1;

	return ( ret );
}


int	init_query( qp )
Query	*qp;
{
	if ( qp == NULL )
		return ( FALSE );

	if ( qp->vartype == VAL_STRING ) {
#ifdef USE_REGEX
		if ( qp->RE ) {
			REfree( qp->RE );
			qp->RE = NULL;
		}
/*
		if ( strlen( qp->str ) == 0 ) {
			strcpy( qp->str, ".*" );
		}
*/
#endif /* USE_REGEX */
	}
	qp->keylen = strlen( qp->str );
	qp->MatchType = 0;

	return ( TRUE );
}


int	parse_query( qp, isTarget )
Query	*qp;
int	isTarget;
{
	register char	*qs = qp->str,	*p1;
	int				v_type = qp->vartype;
	char			wk[KEYSTR_MAX+1];

	if ( qp == NULL )
		return ( FALSE );

	qp->MatchType = qp->keylen = 0;
	if ( isTarget ) {
		clip_blanks( qs );
		if ( v_type == VAL_STRING ) {
#ifdef USE_REGEX
			if ( strlen( qs ) ) {
				if ( ( qp->RE = REcomp( qs ) ) == NULL ) {
					strcpy( qs, "" );
				} else {
					qp->MatchType = ( MATCH_RE | ( MATCH_RE << 8 ) );
				}
			}
#endif /* USE_REGEX */
			qp->keylen = strlen( qs );
		} else {
			if ( strlen(qs) ) {
				strncpy( wk, qp->str, KEYSTR_MAX );
				wk[KEYSTR_MAX] = '\0';
				qs = wk;
				if ( ( p1 = strchr( qs, ':' ) ) != NULL ) {		/* range */
					qp->MatchType  = ( MATCH_LT | MATCH_EQ ) << 8;
					qp->MatchType |= ( MATCH_LT | MATCH_EQ );
					*p1 = '\0',	p1++;
					str2val( p1, v_type, &qp->max );
					str2val( qs, v_type, &qp->min );
					return ( TRUE );
				}
				if ( ( p1 = strchr( qs, '>' ) ) != NULL ) {
					qp->MatchType |= MATCH_GT;
					*p1 = '\0',	p1++;
					if ( *p1 == '=' ) {
						qp->MatchType |= MATCH_EQ;
						*p1 = '\0',	p1++;
					}
					str2val( p1, v_type, &qp->min );
					return ( TRUE );
				}
				if ( ( p1 = strchr( qs, '<' ) ) != NULL ) {
					qp->MatchType |= ( MATCH_LT << 8 );
					*p1 = '\0',	p1++;
					if ( *p1 == '=' ) {
						qp->MatchType |= ( MATCH_EQ << 8 );
						*p1 = '\0',	p1++;
					}
					str2val( p1, v_type, &qp->max );
					return ( TRUE );
				}
				/* match the value itself */
				qp->MatchType |= ( MATCH_EQ | ( MATCH_EQ << 8 ) );
				str2val( qs, v_type, &qp->min );
				qp->max = qp->min;
				return ( TRUE );
			}
		}
	}

	return ( TRUE );
}


int
match_query( qp )
Query	*qp;
{
	unsigned short	HasMin,	HasMax;
	int				type,	w;
	int				result = TRUE;

	type = qp->MatchType;
	HasMin =   type        & MATCH_ALL;
	HasMax = ( type >> 8 ) & MATCH_ALL;
	result = TRUE;		/* Maybe */
	if ( qp->vartype == VAL_DEGREE || qp->vartype == VAL_RA ) {
		if ( HasMin && HasMax && qp->min > qp->max ) {
			if ( valpcmp( qp->varptr, &qp->min, qp->vartype ) >= 0
			  || valpcmp( qp->varptr, &qp->max, qp->vartype ) <= 0 ) {
				return ( TRUE );
			} else {
				return ( FALSE );
			}
		}
	}
	if ( HasMin ) {
		if ( qp->vartype == VAL_STRING ) {
#ifdef	USE_REGEX
			if ( REmatch( qp->RE, (char *)qp->varptr ) == FALSE )
				result = FALSE;
#else
			if ( strstr( qp->varptr, qp->str ) == NULL )
				result = FALSE;
#endif	/* USE_REGEX */
		} else {
			w = valpcmp( qp->varptr, &qp->min, qp->vartype );
			if (   w <  0
			  || ( w == 0 && ! ( HasMin & MATCH_EQ ) ) )
				result = FALSE;
		}
	}
	if ( result && HasMax ) {
		if ( HasMax & MATCH_RE ) {		/* ignore this case */
			;
		} else {
			w = valpcmp( qp->varptr, &qp->max, qp->vartype );
			if (   w >  0
			  || ( w == 0 && ! ( HasMax & MATCH_EQ ) ) )
				result = FALSE;
		}
	}

	return ( result );
}

/* **************  End of getval.c  ************* */
