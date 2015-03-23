/*
 *	eph_more.c
 *		EPH terminal I/O routines
 *			with ncurses library
 *
 */

#include <curses.h>

#include "eph.h"
#include "menu.h"
#include "ephcurs.h"

/*
 *	for 'more' function
 */
int more_cmd( guide_level )
     int	guide_level;
{
  int		ret = MORE_UNKNOWN;
  CursPos	curspos;

  push_cursor(&curspos);

  lower_left();
  reverse_start();
  dos_puts( "--- more ---" );
  reverse_end();
  guidance( GUID_MORE );

  while ( ret == MORE_UNKNOWN ) {
    switch ( getchr() ) {
    case CR:
    case DOWN_ARROW:
    case 'j':
    case 'e':
      ret = MORE_1_LINE;
      break;
    case ' ':
    case RIGHT_ARROW:
    case ctrl('V'):
    case 'f':
      ret = MORE_1_PAGE;
      break;
    case 'x':
      ret = MORE_ALL;
      break;
    case 'q':
    case ESCAPE:
      ret = MORE_QUIT;
      break;
    default:
      ret = MORE_UNKNOWN;
    }
  }
  lower_left();
  clear_eol( 0 );
  guidance( guide_level );

  pop_cursor(&curspos);

  return ( ret );
}

/* ****************  end of eph_more.c  **************** */
