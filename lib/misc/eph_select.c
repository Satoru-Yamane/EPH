/*
 *	eph_select.c
 *		eph_select 0 ~ 10 values
 *
 *	(C)opyright 1988, All right reserved
 *				by S.Yamane
 *
 *	$Header: SELECT.Cv  1.9  90/12/27 15:34:58  yamane  Rel $
 */

#include	<stdio.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"ephcurs.h"


#define		SELECT			0
#define		DESELECT		1


/*
 *	functions
 */
static void	print_selstr(Select *item, int n, int mode)
{
	int		x,	y;

	x = item->xpos + n * item->xstep;
	y = item->ypos + Y_BIAS;
	moveto( x, y );
	if ( mode == SELECT )
		reverse_start();

	dos_puts( item->selstr[n] );

	if ( mode == SELECT )
		reverse_end();
}

int	disp_val(Select *item)
{
	register int	i,	mode;

	for ( i = 0; i < 10 && item->selstr[i] != NULL; i++ ) {
		mode = ( *(item->sw_var) == i ) ? SELECT : DESELECT;
		print_selstr( item, i, mode );
	}

	return ( i );
}

/*
 *	eph_select ---
 */
MenuFunc
eph_select( item )
Select	*item;
{
	int		max,	ik,	done;
	int		wv;
	int		retval;
	CursPos	curs_pos;

	push_cursor(&curs_pos);
	clear_keybuf();

	wv = *(item->sw_var);	/* reserve original value */
	max = disp_val( item );
	do {
		done = 0;
		print_selstr( item, wv, SELECT );
		ik = getchr();
		print_selstr( item, wv, DESELECT );
		switch ( ik ) {
		case ' ':
		case TAB:
		case RIGHT_ARROW:
				if( ++wv > max-1 )
					wv = 0;
				break;
		case BS:
		case LEFT_ARROW:
				if( --wv < 0 )
					wv = max-1;
				break;
		case ctrl('C'):
				ik = ESCAPE;
				/* No break */
		case ESCAPE:
				done = 1;
				break;
		case CR:
		case UP_ARROW:
		case DOWN_ARROW:
				done = 1;
				*(item->sw_var) = wv;
				break;
		default:
				RingBell();
				break;
		}
	} while ( ! done );
	disp_val( item );
	pop_cursor(&curs_pos);

	retval = ( ik == UP_ARROW ? FORE_ITEM : NEXT_ITEM ) | NO_REWRITE;

	return ( ( ik == ESCAPE ? menu_upper() : retval ) );
}

/* *************  end of eph_select.c  ************** */
