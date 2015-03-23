/*
 *	sel_dev.c :
 *		Select output device for EPH
 *
 *	(C)opyright 1989-1990,  all right reserved
 *	    Feb. 6, 1989      by S.Yamane
 *      Modified          by K.Osada ( Dec.17, 1989 )
 *
 *	$Header: SEL_DEV.Cv  1.7  90/12/07 10:00:46  yamane  Rel $
 */

#include	<stdio.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"ephcurs.h"

#define	NAME_SPACE			42

/*
 *	sel_dev ---
 *		select devices switch
 */
MenuFunc
sel_dev( param )
Select	*param;
{
	register int	x,	y;
	int			retval;
	Fname		fname;

	retval = eph_select( param );
	if ( retval == UPPER_MENU || *(param->sw_var) != DEV_FILE ) {
		return ( retval );
	}
	x = param->xpos - 20;
	y = param->ypos +  6;

	frame_box(x-2, y, NAME_SPACE+2, 3);
	moveto( x-1, y+1 );
	EPH_Hline( ' ', NAME_SPACE );

	fname.xpos = x;
	fname.ypos = y + 1 - Y_BIAS;
	fname.flg  = 1;			/* create */

	switch ( global_sw.exec_mode ) {
	case EXEC_MAIN:
		switch ( ctrl_sw.ent_obj ) {
			case COMET_IN:
				fname.varptr = current.w_orb_comet;
				break;
			default:
				fname.varptr = current.w_orb_mpla;
				break;
		}
		break;
	case EXEC_EPH:
		fname.varptr = current.w_eph_file;
		break;
	case EXEC_IDENT:
		switch ( ctrl_sw.id_out_mode ) {
			case ID_O_HA:
			case ID_O_OC:
				fname.varptr = current.w_id_file;
				break;
			default:
				fname.varptr = current.w_id_file;
				break;
			}
			break;
	default:
		warning( "Internal error!!" );
		return ( menu_upper() );
	}

	moveto( x, y+1 );
	dos_puts( fname.varptr );
	retval = get_fname( &fname );

	frame_box(x-2, y, NAME_SPACE+2, 3);
	moveto( x-1, y+1 );
	EPH_Hline( ' ', 42 );

	return ( retval | ( ! NO_REWRITE ) );
}

/* ******************  end of sel_dev.c  ****************** */
