/*
 *	getval.h :
 *		header file for getval/putval
 *
 *	(C)opyright 1990,  all right reserved
 *		Dec.28, 1990      by S.Yamane
 *
 *	$Header: GETVAL.Hv  1.3  91/01/11 16:48:28  yamane  Rel $
 */

#ifndef	_GETVAL_DEF_

#define	_GETVAL_DEF_

#include	"menu.h"
#ifdef	USE_REGEX
#	define	BYTEWIDTH			8
#	if	defined(_WIN32)
#		if	__TURBOC__
#			include <alloc.h>
#		else
#			include <malloc.h>
#		endif
#	endif
#	include	"regex.h"
	typedef struct re_pattern_buffer	RePat;
#endif

typedef struct _Field_ {
	void	*varptr;
	int	vartype;
	int	x,	y;
	int	field_len;
	char	*disp_fmt;
	char	*ed_fmt;
	int	(*after_edit)( struct _Field_ * );
	int	changed;
	short	hyphen;		/* bit mask : *sssss-----sssss */
} Field;

#define  HYPHEN( sp1, hyp, sp2 )	( (sp1)<<10 | (hyp)<<5 | (sp2) )


#define	KEYSTR_MAX	30

typedef struct	{
	void		*varptr;
	int		vartype;
#ifdef USE_REGEX
	RePat		*RE;
#endif
	double		min;
	double		max;
	unsigned short	MatchType;		/* for numeric variable */
	int		keylen;
	char		str[KEYSTR_MAX+1];
} Query;

/*
 *	value type
 */
#define	VAL_STRING	0x0001
#define	VAL_INTEGER	0x0002
#define	VAL_DOUBLE	0x0004
#define	VAL_SHORT_JD	0x0010		/* YYYY/MM/DD[.D...] */
#define	VAL_LONG_JD	0x0020		/* YYYY/MM/DD HH:MM:SS */
#define	VAL_TIME	0x0040		/* HH:MM:SS[.S...] */
#define	VAL_DEGREE	0x0100
#define	VAL_RA		0x0200		/* HH MM SS[.S...] */
#define	VAL_DECL	0x0400		/* DD MM SS[.S...] */

/*
 *	query matching type
 *		for minimum value.
 *		for maximum value, shift 8 bit left
 */
#define	MATCH_LT	0x0001
#define	MATCH_EQ	0x0002
#define	MATCH_GT	0x0004
#define	MATCH_RE	0x0008
#define	MATCH_ALL	( MATCH_LT | MATCH_EQ | MATCH_GT | MATCH_RE )

/*
 *	function prototypes
 */
extern char	*putval( Field *param, int dim );
extern MenuFunc	getval( Field *param );
#ifdef	USE_REGEX
extern RePat	*REcomp( char *pat );
extern int	REfree( RePat *cpat );
extern int	REmatch( RePat *cpat, char *str );
#endif
extern int	str2val( char *s, int vartype, void *varptr );
extern int	val2str( void *varptr, int vartype, char *s );
extern int	valpcmp( void *p1, void *p2, int type );
extern int	init_query( Query *qp );
extern int	parse_query( Query *qp, int istarget );
extern int	match_query( Query *qp );

/* defined getval.c */
extern int valsprintf(char *, char *, int, void *);

#endif	/* _GETVAL_DEF_ */

/* ************  end of getval.h  ************** */
