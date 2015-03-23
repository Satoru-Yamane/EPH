/*
 *	fmt_gets.c
 *		formatted entry routine
 *
 *	(C)opyright 1989-1990,  all right reserved
 *	   Jan. 10, 1989     by S.Yamane
 *
 *	$Header: FMT_GETS.Cv  1.10  91/01/11 16:06:04  yamane  Rel $
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"eph.h"
#include	"ephcurs.h"
#include	"menu.h"

#ifndef TRUE
#	define	FALSE	0
#	define	TRUE	( ! FALSE )
#endif

/*
 *	define entry-char type
 */
#define		ALL				0
#define		SIGN			1
#define		NUMERIC			2
#define		LCASE			3
#define		UCASE			4
#define		SKIP			5

static int	fctype(char	c)
/*
static int	fctype( c )
char	c;
*/
{
	int		type;

	switch ( c ) {
	case '?':	type = ALL;			break;
	case '+':	type = SIGN;		break;
	case '#':	type = NUMERIC;		break;
	case 'x':	type = LCASE;		break;
	case 'X':	type = UCASE;		break;
	default:	type = SKIP;		break;
	}

	return ( type );
}

/*
 *	ins_char()
 *		make n bytes room to insert.(contents is unknown)
 *		trailer n characters will be lost.
 *
#if KANJI
 *		文字列の p バイト目に n バイト挿入スペースを作る.
 *		文字列の長さは変わらないのではみ出した分は消える.
 * 		挿入された箇所の文字は不定.
#endif
 */
static void	ins_char(unsigned char *s, int p, int n)
{
	register int	len,	i;

	len = strlen((char *)s);
	for ( i = len - 1; i >= p + n; i-- ) {
		s[i] = s[i-n];
	}
#if KANJI
	if ( nthctype( s, len-1 ) == CT_KJ1 )
		s[len-1] = ' ';
#endif
	s[len] = '\0';
}

/*
 *	del_char()
 *		delete n bytes at p and forward trailing characters.
 *		fill n blanks to keep string length constant.
 *
#if KANJI
 *		文字列の p バイト目から n バイト削除して前に詰める.
 *		文字列の長さは変わらないので足りない分は ' ' を補う.
#endif
 */
static void	del_char(unsigned char *s, int p, int n)
{
	register int	len,	i;

	len = strlen((char *)s);
	strncpy((char *)s + p, (char *)s + p + n, len - p + n - 1 );
	for (i = len - n; i < len; i++)
		s[i] = ' ';
	s[len] = '\0';
}

static unsigned short	getchr16(void)
{
	unsigned short	ret;

	ret = getchr();
#if KANJI					/* S-JIS only */
	if ( iskanji(ret) ) {
		ret <<= 8;
		ret |= ( getchr() & 0x00ff );
	}
#endif

	return ( ret );
}



/*
 *	fmt_gets ---
 *		gets with format
 */
unsigned short
fmt_gets( x, y, picture, retstr, edit_len )
int		x,	y;
char	*picture;		/* entry format  */
char	*retstr;	/* output string */
int	edit_len;
{
	register int	p0;
	unsigned short			ik;				/* return value */
	int						all_num,	byte;
	int						err,	ins_mode,	redisp_all;
	static unsigned char	edit_buf[ 256 ];
	char					wfmt[ 50 ];
#if	KANJI
	unsigned char			high;
#endif	/* KANJI */
	unsigned char			low;
	CursPos	curs_pos;

	if ( edit_len > 255 )
		return ( 0xffff );

	sprintf( wfmt, "%%-%d.%ds", edit_len, edit_len );

reedit:
	sprintf( edit_buf, wfmt, retstr );
	p0 = 0;
	ins_mode = 1;

	push_cursor(&curs_pos);

	if ( picture != NULL ) {
		all_num = 1;
		while ( picture[p0] ) {
			if ( fctype(picture[p0]) == ALL
			  || fctype(picture[p0]) == LCASE
			  || fctype(picture[p0]) == UCASE ) {
				all_num = 0;
				break;
			}
			p0++;
		}
		p0 = 0;
		while ( picture[p0] && fctype(picture[p0]) == SKIP ) {
			edit_buf[p0] = picture[p0];
			p0++;
		}
		if ( all_num ) {
			while ( picture[p0] && fctype(picture[p0]) == NUMERIC
			  && edit_buf[p0+1] && edit_buf[p0+1] == ' ' ) {
				p0++;
			}
			if ( picture[p0] && fctype(picture[p0]) == SIGN ) {
				edit_buf[p0] = ( edit_buf[p0] == '-' ) ? '-' : ' ';
				p0++;
			}
		}
		ins_mode = 0;
	}
	redisp_all = TRUE;
	while ( p0 <= edit_len ) {
		if ( redisp_all ) {
			moveto( x, y );
			dos_puts( edit_buf );
			redisp_all = FALSE;
		}
		moveto( x+p0, y );
		disp_cursor();
		ik = getchr16();		/* 16 bit getchr() for KANJI */
#if	KANJI
		high = ( ik >> 8 ) & 0xff;
#endif	/* KANJI */
		low  =   ik        & 0xff;
		ik = low;
		erase_cursor();

		if ( picture && low == BS )
			low = LEFT_ARROW;			/* only back space */

		err = 0;
		switch ( low ) {
		case REEDIT_KEY:
				goto reedit;			/* edit again */

		case CANCEL_KEY:
				goto cancel;
		case INSERT_KEY:				/* overwrite / insert */
				if ( picture == NULL )
					ins_mode = ! ins_mode;
				else
					err = 1;
				break;
		case BS:					/* delete 1 character and trunk */
				if ( picture != NULL && fctype(picture[p0-1]) != ALL ) {
					err = 1;
					break;
				}
				/* else No break */
		case LEFT_ARROW:
				if ( p0 == 0 )
					break;
				byte = 1;
#if	KANJI
				if ( p0 > 1  &&  nthctype( edit_buf, p0-2 ) == CT_KJ1 ) {
					byte = 2;
				}
#endif	/* KANJI */
				p0 -= byte;
				if ( low == LEFT_ARROW )
					break;
				/* else No break : case BS */
		case DELETE_KEY:				/* delete 1 character on cursor */
				byte = 1;
#if	KANJI
				byte = ( iskanji( edit_buf[p0] ) ) ? 2 : 1;
#endif	/* KANJI */
				if ( picture == NULL ) {
					del_char( edit_buf, p0, byte );
					redisp_all = TRUE;
				} else if ( low == ctrl('G') ) {
					err = 1;
				}
				break;
		case RIGHT_ARROW:
				byte = 1;
#if	KANJI
				if ( nthctype( edit_buf, p0 ) == CT_KJ1 )
					byte = 2;
#endif	/* KANJI */
				if ( p0 < edit_len - 1
				  && ( byte == 1 || ( byte == 2 && p0 < edit_len - 2 ) ) ) {
					p0 += byte;
				}
				break;
		case CLEAR_KEY:
				while ( p0 < edit_len ) {
					edit_buf[p0] = ' ';
					p0++;
				}
				/* No break */
		case TAB:
		case UP_ARROW:
		case DOWN_ARROW:
		case CR:
		case ESCAPE:
				edit_buf[edit_len] = '\0';
				goto done;
		default:
				err = 1;
				byte = 1;
#if	KANJI
				if ( high )
					byte = 2;
#endif	/* KANJI */

				if ( picture == NULL ) {
					if ( p0 < edit_len - byte
					  && ! iscntrl(low) )
						err = 0;
				} else {
					switch ( fctype( picture[p0] ) ) {
					case NUMERIC:
							if ( isdigit(low) )
								err = 0;
							/* No break */
					case SIGN:
							if ( low == '+' || low == '-' || low == ' ' )
								err = 0;
							break;
					case ALL:
							if ( ! iscntrl(low) )
								err = 0;
							break;
					case LCASE:
							if ( isalpha(low) && isupper(low) )
								low = tolower(low),  err=0;
							break;
					case UCASE:
							if ( isalpha(low) && islower(low) )
								low = toupper(low),  err=0;
							break;
					}
				}
				if ( err ) {
					RingBell();
				} else {
					redisp_all = TRUE;
					if ( ins_mode ) {
						ins_char( edit_buf, p0, byte );
					}
#if	KANJI
					if ( ! ins_mode ) {		/* 非漢字を漢字にオーバーライト */
						if ( high == 0
						  && nthctype( edit_buf, p0 )   == CT_KJ1
						  && nthctype( edit_buf, p0+1 ) == CT_KJ2 ) {
							del_char( edit_buf, p0, 1 );
						}
					}
					if ( byte == 2 ) {
						edit_buf[ p0 ] = high;
						p0++;
					}
#endif	/* KANJI */
					edit_buf[ p0 ] = low;
					p0++;
				}

				if ( picture ) {
					while ( picture[p0] && fctype( picture[p0] ) == SKIP ) {
						edit_buf[p0] = picture[p0];
						p0++;
					}
				}
				break;
		}
	}
done:
	sprintf( retstr, wfmt, edit_buf );

cancel:
	pop_cursor(&curs_pos);
	moveto( x, y );
	dos_puts( retstr );

	return ( low );
}

/* ***************  end of fmt_gets.c  ************** */
