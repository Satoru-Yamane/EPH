/*
 *	eph_curses.c
 *		EPH terminal I/O routines
 *			with ncurses library
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <curses.h>
#ifndef _MSC_VER
#include <term.h>
#endif

#include "astrolib.h"
#include "eph.h"
#include "menu.h"
#include "ephcurs.h"

#define		MINIMUM_COLS	80
#define		MINIMUM_LINES	24

/*
 *	global variables
 */
int	EPH_cols = 80;		/* COLUMNS */
int	EPH_lines = 25;		/* LINES */
int	isDumb = FALSE;		/* dumb terminal ? */
int	EPH_ColorMode = FALSE;	/* Terminal Color Mode */

/*
 *	local variables
 */
static int	ttysetup = 0;

#if 0	/* Not used: 2011/02/06 yamane */
/*
 *	arrow key assignment
 */
static char	*kl = KEY_LEFT;			/* Left Arraw */
static char	*kr = KEY_RIGHT;		/* Right Arraw */
static char	*ku = KEY_UP;			/* Up Arraw */
static char	*kd = KEY_DOWN;			/* Down Arraw */

/*
 *	special keys
 */
static char	*kD = KEY_DC;			/* Delete Key */
static char	*kI = KEY_IC;			/* Insert Key */
#endif

/*
 *	functions
 */
/*
 * --------------------
 *	Output functions
 * --------------------
 */
/*
 *	dos_putc ---
 *		actual putc function for DOS
 */

int dos_putc( c )
     char	c;
{
  return ( addch( c ) );
}

/*
 *	dos_puts ---
 *		put string using dos_putc()
 */
int dos_puts( s )
     char	*s;
{
  return ( printw("%s", s) );
}

/*
 *	dos_printf ---
 *		printf function for using dos_putc()
 */
int dos_printf( char *fmt, ... )
{
  char		buf[200];
  va_list		arg_ptr;
  
  va_start( arg_ptr, fmt );
  vsprintf( buf, fmt, arg_ptr );
  va_end( arg_ptr );
  
  return ( dos_puts( buf ) );
}

/*
 *	EPH_Hline ---
 *		draw horizontal line
 */
int EPH_Hline( c, leng )
     char	c;
     int	leng;
{
  return ( hline( c, leng) );
}

/*
 *	EPH_Vline ---
 *		draw vertical line
 */
int EPH_Vline( c, leng )
     char	c;
     int	leng;
{
  return ( vline( c, leng) );
}

/*
 * --------------------
 *	Input functions
 * --------------------
 */

/*
 *	getchr ---
 *		read the next char, blocking if necessary, and return it. don't echo.
 *		map the arrow keys if we can too into hjkl
 */
int getchr()
{
  int c;

  c = getch();
  /*
   *	change some key
   * convert arrow key sequence if exists
   */
  switch ( c ) {
  case KEY_RIGHT:	return ( RIGHT_ARROW );
  case KEY_LEFT:	return ( LEFT_ARROW );
  case KEY_UP:		return ( UP_ARROW );
  case KEY_DOWN:	return ( DOWN_ARROW );
  case KEY_DC:		return ( DELETE_KEY );
  case KEY_IC:		return ( INSERT_KEY );
  case 0x12:		return ( REEDIT_KEY );
  case 0x03:		return ( CANCEL_KEY );
  case 0x09:		return ( TAB );
  case 0x0a:		return ( CR );
  case 0x0b:		return ( CLEAR_KEY );
  case KEY_BACKSPACE:	return ( BS );

  }

  return ( c );
}

/*
 *    input keybord flush
 */
int clear_keybuf()
{
  flushinp();
  return(0);
}

/*
 * -----------------------------
 *	curses handling functions
 * -----------------------------
 */
void	EPH_SetColor( EPH_Color )
     short	EPH_Color;
{
    color_set(EPH_Color, NULL);
}

/*
 *	clear function
 */
/*	invisible cursor	*/
int erase_cursor()
{
#ifdef _WIN32
  curs_set(0);
#else
  putp(cursor_invisible);         /* Set cursor invisible */
#endif
  return(0);
}

int disp_cursor()
{
#ifdef _WIN32
  curs_set(2);
#else
  putp(cursor_normal);		/* Set cursor visible */
#endif
  return(0);
}

int clear_eol( x )
     int	x;
{
  clrtoeol();
  return(0);
}

int clear_bottom( x, y )
     int	x,	y;
{
  register int	yy = y;

  moveto( x, y );
  clrtobot();
  return(0);
}

int ClearScreen()
{
  clear();
  return(0);
}


/*
 *	cursor move function
 */
int moveto( x, y )					/* cm */
     int	x,	y;
{
  move(y, x);
  return(0);
}

/*	ring bell	*/
int RingBell()
{
  beep();
  return(0);
}

int CursorUp()
{
  int y, x;

  getyx(stdscr, y, x);
  if(y > 0) {
    move(y - 1, x);
    }
  return(0);
}


#if 0		/* ********************************* */
int CursorDown()
{
  getyx(stdwin, y, x);
  if(y < 23) {
    move(y + 1, x);
    }
  return(0);
}

int cursor_forward()
{
  getyx(stdwin, y, x);
  if(x <79) {
    move(y, x + 1);
    }
  return(0);
}

int cursor_backward()
{
  getyx(stdwin, y, x);
  if(x > 0) {
    move(y, x - 1);
    }
  return(0);
}
#endif		/* ********************************* */

/*	save cursor	*/
int push_cursor(curspos)
    CursPos	*curspos;
{
/*
  getsyx(curspos->y, curspos->x);
*/
  return(0);
}

/*	restore curosr	*/
int pop_cursor(curspos)
    CursPos	*curspos;
{
/*
  setsyx(curspos->y, curspos->x);
*/
  return(0);
}


/*
 *	video attribute control function
 */
int reverse_start()		/* Reverse start */
{
  attron(A_REVERSE);
  return(0);
}

int reverse_end()		/* Reverse end */
{
  attroff(A_REVERSE);
  return(0);
}

int blink_start()		/* Blink start */
{
  attron(A_BLINK);
  return(0);
}
int blink_end()		/* Blink end */
{
  attroff(A_BLINK);
  return(0);
}

int invisible_start()		/* Invisible start */
{
  attron(A_INVIS);
  return(0);
}

int bold_start()		/* bold start */
{
  if (tigetstr("bold"))
    attron(A_BOLD);
  else if (EPH_ColorMode)
    EPH_SetColor(EPH_YELLOW);
  return(0);
}
int bold_end()			/* bold end */
{
  if (tigetstr("bold"))
    attroff(A_BOLD);
  else if (EPH_ColorMode) {
#ifdef _WIN32
    EPH_SetColor(EPH_WHITE);
#else
    EPH_SetColor(EPH_BLACK);
#endif
    use_default_colors();
    }
  return(0);
}

int halfbright_start()		/* half-bright start */
{
  if (tigetstr("dim"))
    attron(A_DIM);
  else if (EPH_ColorMode)
#ifdef _WIN32
    EPH_SetColor(EPH_CYAN);
#else
    EPH_SetColor(EPH_BLUE);
#endif
  return(0);
}

int halfbright_end()		/* half-bright end */
{
  if (tigetstr("dim"))
    attroff(A_DIM);
  else if (EPH_ColorMode) {
#ifdef _WIN32
    EPH_SetColor(EPH_WHITE);
#else
    EPH_SetColor(EPH_BLACK);
#endif
    use_default_colors();
    }
  return(0);
}

int modify_end()		/* Normal mode| All Attribute reset */
{
  attrset(A_NORMAL);
  return(0);
}

int underline_start()		/* Under-line start */
{
#ifdef _WIN32
  if (EPH_ColorMode) {
    EPH_SetColor(EPH_CYAN);
    }
#else
  attron(A_UNDERLINE);
#endif
  return(0);
}

int underline_end()		/* Under-line end */
{
#ifdef _WIN32
  if (EPH_ColorMode) {
    EPH_SetColor(EPH_WHITE);
    }
#else
  attroff(A_UNDERLINE);
#endif
  return(0);
}

int standout_start()		/* Standout start */
{
  standout();
  return(0);
}

int standout_end()		/* Standout end */
{
  standend();
  return(0);
}


int ins_line()			/* Insert line */
{
  insertln();
  return(0);
}

int del_line()			/* Delete line */
{
  deleteln();
  return(0);
}

int lower_left()		/* Lower left */
{
  moveto( 0, EPH_lines-1 );
  return(0);
}

int scroll_up( top, nlines )	/* Scroll Up */
     int	top,	nlines;
{
  register int	i;
  CursPos	curspos;

  push_cursor(&curspos);
  moveto( 0, top );
  for ( i = nlines; i; i-- ) {
    del_line();
  }
  pop_cursor(&curspos);
  for ( i = nlines; i; i-- ) {
    CursorUp();
  }
  return(0);
}

int scroll_down( top, nlines )	/* Scroll Down */
     int	top,	nlines;
{
  register int	i;
  CursPos	curspos;

  push_cursor(&curspos);
  moveto( 0, top );
  for ( i = nlines; i; i-- ) {
    ins_line();
  }
  pop_cursor(&curspos);
  return(0);
}


/*
 *	eph_term_init ---
 *		initialize and setup terminal
 */
int eph_term_init( )
{
  int	ret;
  char	*entry = NULL;
  char	*cap;
  short color01;

  ret = 0;
  initscr();			/* Init sceen */
  EPH_cols = COLS;
  EPH_lines = LINES;
  if(EPH_cols < MINIMUM_COLS || EPH_lines < MINIMUM_LINES ) {
    eph_term_deinit();
    fputs("Screen is too narrow.\n", stderr);
    fprintf(stderr, "Required COLS:%d/LINES:%d but currently COLS:%d/LINES:%d.\n",
    		MINIMUM_COLS, MINIMUM_LINES, EPH_cols, EPH_lines);
    fputs("Sorry...\n", stderr);
    ret = 1;
    return ( ret );
  }
  cbreak();			/* Non-Blocking Mode */
  noecho();			/* No echo */
#ifdef ENABLE_COLOR
  if(has_colors()) {
    EPH_ColorMode = 1;
    start_color();                /* Start color */
    assume_default_colors(COLOR_BLACK, -1);
#ifdef _WIN32
    init_pair( 0, COLOR_BLACK,	COLOR_BLACK);
    init_pair( 1, COLOR_RED,	COLOR_BLACK);
    init_pair( 2, COLOR_GREEN,	COLOR_BLACK);
    init_pair( 3, COLOR_YELLOW,	COLOR_BLACK);
    init_pair( 4, COLOR_BLUE,	COLOR_BLACK);
    init_pair( 5, COLOR_MAGENTA,COLOR_BLACK);
    init_pair( 6, COLOR_CYAN,	COLOR_BLACK);
    init_pair( 7, COLOR_WHITE,	COLOR_BLACK);
#else
    init_pair( 0, COLOR_BLACK,	-1);
    init_pair( 1, COLOR_RED,	-1);
    init_pair( 2, COLOR_GREEN,	-1);
    init_pair( 3, COLOR_YELLOW,	-1);
    init_pair( 4, COLOR_BLUE,	-1);
    init_pair( 5, COLOR_MAGENTA,-1);
    init_pair( 6, COLOR_CYAN,	-1);
    init_pair( 7, COLOR_WHITE,	-1);
#endif
    init_pair( 8, COLOR_BLACK,	COLOR_BLACK);
    init_pair( 9, COLOR_BLACK,	COLOR_RED);
    init_pair(10, COLOR_BLACK,	COLOR_GREEN);
    init_pair(11, COLOR_BLACK,	COLOR_YELLOW);
    init_pair(12, COLOR_BLACK,	COLOR_BLUE);
    init_pair(13, COLOR_BLACK,	COLOR_MAGENTA);
    init_pair(14, COLOR_BLACK,	COLOR_CYAN);
#ifdef _WIN32
    init_pair(15, COLOR_BLACK,	COLOR_WHITE);
#else
    init_pair(15, COLOR_BLACK,	-1);
#endif
    init_pair(16, COLOR_WHITE,	COLOR_BLACK);
    init_pair(17, COLOR_WHITE,	COLOR_RED);
    init_pair(18, COLOR_WHITE,	COLOR_GREEN);
    init_pair(19, COLOR_WHITE,	COLOR_YELLOW);
    init_pair(20, COLOR_WHITE,	COLOR_BLUE);
    init_pair(21, COLOR_WHITE,	COLOR_MAGENTA);
    init_pair(22, COLOR_WHITE,	COLOR_CYAN);
#ifdef _WIN32
    init_pair(23, COLOR_WHITE,	COLOR_WHITE);
#else
    init_pair(23, COLOR_WHITE,	-1);
#endif
#ifdef _WIN32
    color_set(7, 0);
#else
    color_set(0, NULL);
#endif
    }
#endif
  keypad(stdscr, TRUE);		/* arraw key enable */
#if !defined (_WIN32) && !defined(__CYGWIN__) && !defined(__MINGW32__)
  if (getenv("ESCDELAY") == NULL) ESCDELAY = 50;
#endif
  erase_cursor();
  return ( ret );
}


/*
 *	eph_term_deinit ---
 *		de-init terminal
 */
int eph_term_deinit()
{
  erase();
  refresh();
  endwin();
  return(0);
}

/*
 *  frame_box - draw frame box.
 */
void frame_box(x_pos, y_pos, width, height)
int x_pos, y_pos, width, height;
{
  int i;
#ifdef ENABLE_SYMBOL
  move(y_pos, x_pos);				hline(ACS_ULCORNER, 1);
  move(y_pos, x_pos + 1);			hline(ACS_HLINE, width - 2);
  move(y_pos + 1, x_pos);			vline(ACS_VLINE, height - 2);
  move(y_pos + height - 1, x_pos);		hline(ACS_LLCORNER, 1);
  move(y_pos, x_pos + width - 1);		hline(ACS_URCORNER, 1);
  move(y_pos + 1, x_pos + width - 1);		vline(ACS_VLINE, height - 2);
  move(y_pos + height - 1, x_pos + 1);		hline(ACS_HLINE, width - 2);
  move(y_pos + height - 1, x_pos + width - 1);	hline(ACS_LRCORNER, 1);

  for(i = 1; i < height - 1; i++) {
    move(y_pos + i, x_pos + 1);         hline(' ', width - 2);
    }
#else
  reverse_start();
  move(y_pos, x_pos);                           hline(' ', width);
  move(y_pos + 1, x_pos);                       vline(' ', height - 2);
  move(y_pos + 1, x_pos + width - 1);           vline(' ', height - 2);
  move(y_pos + height - 1, x_pos);              hline(' ', width);
  reverse_end();

  for(i = 1; i < height - 2; i++) {
    move(y_pos + i, x_pos + 1);         hline(' ', width - 2);
    }
#endif
  }

/*
 *  EPH_getstr - get string
 */
int EPH_getstr(y, x, str, str_size)
int x, y, str_size;
char *str;
{
  int i, j, c, in_leng, ins_sw=0;

  in_leng = strlen(str);
  if(ins_sw) {
    mvprintw(1, 0, "Insert mode                 ");
    } else {
    mvprintw(1, 0, "Overwrite mode              ");
    }
  for(i = 0; i <= str_size; i++) {
    if(i == str_size) { i--; }
    move(y, x + i);
    disp_cursor();
    c = getch();
    if(c == 0x0a || c == 0x09 || c == KEY_UP || c == KEY_DOWN || c == 0x1b) { break; }
    switch(c) {
      case KEY_RIGHT:
	if(i >=  in_leng) { i--; }
	break;
      case KEY_BACKSPACE:
      case KEY_LEFT:
	if(i > 0) { i -= 2; } else  { i--; }
	break;
      case KEY_DC:
	mvprintw(y, x + i , "%s", str + i + 1);
	printw("%s", " ");
	if(in_leng  - 1 > i) {
	  for(j = i; j < in_leng; j++) {
	    str[j] = str[j+1];
	    }
	  }
	i--;
	break;
      case KEY_IC:
	if(ins_sw) {
	  ins_sw = 0;
          mvprintw(1, 0, "Overwrite mode              ");
          } else {
	  ins_sw = 1;
          mvprintw(1, 0, "Insert mode                 ");
          }
	i--;
	break;
      case 0x0b:			/* ^K */
	memset(str+i, 0, in_leng - i);
        move(y, x + i);
	hline(' ', in_leng - i);
	i--;
	break;
      case 0x0a:			/* 0x0a : Enter(CR)	*/
      case 0x09:			/* 0x09 : TAB		*/
      case 0x1b:			/* 0x1b : ESC		*/
      case KEY_UP:
      case KEY_DOWN:
	break;
      default:
	if(ins_sw) {
	  mvprintw(y, x + i + 1, "%s", str + i);
	  for(j = in_leng; j >= i; j--) {
	    str[j] = str[j-1];
	    }
	  }
        move(y, x + i);
	printw("%c", c);
	in_leng++;
	str[i] = c;
      }
    erase_cursor();
    }
  return(c);
  }

/*
 *  my_getnum - get number
 */
void EPH_getnum(y, x, str, str_size)
int x, y, str_size;
char *str;
{
  int i, j, c, in_leng, ins_sw=0;

  in_leng = strlen(str);
  if(ins_sw) {
    mvprintw(1, 0, "Insert mode   ");
    } else {
    mvprintw(1, 0, "Overwrite mode");
    }
  for(i = 0; i <= str_size; i++) {
    if(i == str_size) { i--; }
    move(y, x + i);
    disp_cursor();
    c = getch();
    if(c ==  0x0a || c ==0x09 || c ==KEY_UP || c ==KEY_DOWN) { break; }
    switch(c) {
      case KEY_RIGHT:
	if(i >=  in_leng) { i--; }
	break;
      case KEY_BACKSPACE:
      case KEY_LEFT:
	if(i > 0) { i -= 2; } else  { i--; }
	break;
      case KEY_DC:
	mvprintw(y, x + i , "%s", str + i + 1);
	printw("%s", " ");
	if(in_leng  - 1 > i) {
	  for(j = i; j < in_leng; j++) {
	    str[j] = str[j+1];
	    }
	  }
	i--;
	break;
      case KEY_IC:
	if(ins_sw) {
	  ins_sw = 0;
          mvprintw(1, 0, "Overwrite mode");
          } else {
	  ins_sw = 1;
          mvprintw(1, 0, "Insert mode   ");
          }
	i--;
	break;
      case 0x0b:			/* ^K */
	memset(str+i, 0, in_leng - i);
        move(y, x + i);
	hline(' ', in_leng - i);
	i--;
	break;
      case 0x0a:
      case 0x09:
      case KEY_UP:
      case KEY_DOWN:
	break;
      default:
	if(c < 0x30 || c > 0x39) {
	  i--;
	  break;
	  }
	if(ins_sw) {
	  mvprintw(y, x + i + 1, "%s", str + i);
	  for(j = in_leng; j >= i; j--) {
	    str[j] = str[j-1];
	    }
	  }
        move(y, x + i);
	printw("%c", c);
	in_leng++;
	str[i] = c;
      }
    erase_cursor();
    }
  }

/*
 * EPH_getkey - get control key
 */
int EPH_getkey()
{
  int c, break_sw=1;

  while(break_sw) {
    c = getch();
    switch(c) {
      case KEY_NPAGE:
      case KEY_PPAGE:
      case KEY_UP:
      case KEY_DOWN:
      case 0x44:		/* D  : DOWNLOAD  */
      case 0x64:		/* d  : download  */
      case 0x49:		/* I  : IMPORT    */
      case 0x69:		/* i  : import    */
      case ESCAPE:		/* ESC: QUIT      */
      case 0x51:		/* Q  : QUIT      */
      case 0x71:		/* q  : quit      */
      case 0x0a:		/* CR : ENTER KEY */
      case 0x0b:		/* ^K : Ctrl + K  */
      case 0x20:		/* Space          */
	break_sw = 0;
	break;
      }
    }
  return(c);
  }

/* ****************  end of eph_curses.c  **************** */
