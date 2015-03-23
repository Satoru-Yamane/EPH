/*
 *	menu_man.c :
 *		Menu manager for EPH
 *
 *	(C)opyright 1988-1990,	all right reserved
 *		Dec.21, 1988		by S.Yamane
 *	  Dec.18, 1989	  by K.Osada
 *
 *	$Header: MENU_MAN.Cv  1.14  91/01/17 15:12:52  yamane  Rel $
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

#include	"eph.h"
#include	"ephcurs.h"
#include	"menu.h"

#define		DESELECT		( 0 )
#define		SELECT			( 1 )
#define		CLEAR			( 2 )

/*
 *	functions
 */
MenuFunc
menu_quit()
{
	return ( QUIT );
}

MenuFunc
menu_upper()
{
	return ( UPPER_MENU | NO_REWRITE );
}

MenuFunc
not_supported()
{
	warning( "Sorry, Not supported yet." );

	return ( UPPER_MENU | NO_REWRITE );
}

static void	print_title(Menu *menu, int n, int mode, int type)
{
	int		i;
	char	c = ' ';

	if ( type == MN_INVISIBLE ) {
		i = strlen( (menu+n)->title ) + 6;
		EPH_Hline( ' ', i );
		return;
	}
	if ( (menu+n)->exe_flg == NOT_EXEC ) {
		c = '-';
		halfbright_start();
	} else {
		c = ( mode == SELECT ) ? '*' : ' ';
	}

	if (   type == MN_SELECT
	  || ( type == MN_INPUT  && mode == SELECT ) )
		reverse_start();

	dos_putc( c );

	if ( mode == SELECT ) {
		reverse_end();
	}
	if ( mode == SELECT ) {
		underline_start();
	}
	if ( type == MN_SELECT )
		dos_printf( " %c/%s ", (menu+n)->cmd_key, (menu+n)->title );
	else
		dos_printf( " %s ", (menu+n)->title );

	if ( mode == SELECT )
		underline_end();
	else
		reverse_end();
	if ( (menu+n)->exe_flg == NOT_EXEC ) {
		halfbright_end();
	}

	if ( type == MN_SELECT ) {
		reverse_start();
		dos_putc( ' ' );
		reverse_end();
	}
}


static int	menu_bar(Menu *menu, int x0, int y0, int type)
{
	register int	cnt;

	for ( cnt = 0; cnt < 32 && (menu+cnt)->title != NULL; cnt++ ) {
		moveto( x0, y0+cnt );
		print_title( menu, cnt, DESELECT, type );
	}

	return ( cnt );
}

static int	cmd_no(Menu *menu, char key)
{
	register int	ret;

	if ( islower( key ) )
		key = toupper( key );

	for ( ret = 0; ret < 32 && (menu+ret)->title != NULL; ret++ ) {
		if ( (menu+ret)->cmd_key == key )
			break;
	}

	return ( ret );
}

/*
 *	menu_man ---
 *		Menu selector
 */
MenuFunc
menu_man( menu, x0, y0, type, deflt )
Menu	*menu;
int	x0,	y0;
int	type;		/* menu type */
int	deflt;
{
	register int	cnt,	i;
	int		w,	ik;
	unsigned short	ret_val = NEXT_ITEM;
	CursPos	curs_pos;

	push_cursor(&curs_pos);

	cnt = menu_bar( menu, x0, y0, type );

	i = deflt;
	do {
		clear_keybuf();

		moveto( x0, y0+i );
		print_title( menu, i, SELECT, type );
		moveto( x0, y0+i );		/* position cursor to strings head */
		switch ( (menu+i)->exe_flg ) {
		case NOT_EXEC:
				if ( ret_val & FORE_ITEM )
					ik = UP_ARROW;
				else /* if ( ret_val & NEXT_ITEM ) */
					ik = DOWN_ARROW;
				break;
		case AUTOEXEC:
				ik = CR;
				break;
		case WAIT_KEY:
				ik = getchr();
				break;
		}
		print_title( menu, i, DESELECT, type );

		if ( (menu+i)->exe_flg == WAIT_KEY ) {
			w = cmd_no( menu, ik );
			if ( 0 <= w && w < cnt ) {
				i = w;
				ik = CR;
			}
		}

		switch ( ik ) {
		case ESCAPE:
				ret_val |= UPPER_MENU;
				break;
		case ctrl('C'):
				ret_val |= ( type == MN_INPUT ) ? QUIT : UPPER_MENU;
				break;
		case CR:
				moveto( x0, y0+i );
				print_title( menu, i, SELECT, type );
				ret_val = (*(menu+i)->mfunc)( (menu+i)->param );
				if ( ! ( ret_val & NO_REWRITE ) ) {
					menu_bar( menu, x0, y0, type );
				}
				if ( ret_val & NEXT_ITEM || ret_val & FORE_ITEM ) {
					moveto( x0, y0+i );
					print_title( menu, i, DESELECT, type );
				}
				if ( ret_val & NEXT_ITEM )
					i++;
				if ( ret_val & FORE_ITEM )
					i--;
				break;
		case BS:
		case UP_ARROW:
		case LEFT_ARROW:
				ret_val = FORE_ITEM;
				--i;
				break;
		case ' ':
		case DOWN_ARROW:
		case RIGHT_ARROW:
				ret_val = NEXT_ITEM;
				++i;
				break;
		default:
				;
		}
		if ( i < 0	 )
			for ( i = cnt-1; (menu+i)->exe_flg == NOT_EXEC; i-- )
				;
		if ( i > cnt-1 )
			for ( i = 0;	 (menu+i)->exe_flg == NOT_EXEC; i++ )
				;
	} while ( ! ( ( ret_val & QUIT ) || ( ret_val & UPPER_MENU ) ) );

	cnt = menu_bar( menu, x0, y0, MN_INVISIBLE );
	pop_cursor(&curs_pos);


	return ( ret_val | ( i << 5 ) );
}

/* **************   end of menu_man.c  **************** */
