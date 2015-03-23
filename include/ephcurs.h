/*
 *	ephcurs.h :
 *		header for EPH
 *
 */

#ifndef	_EPHCURS_DEF_
#define	_EPHCURS_DEF_

typedef struct _CursPos {	/* Cursor Position */
	int		x,	y;
} CursPos;

#define EPH_BLACK		 0
#define EPH_RED			 1
#define EPH_GREEN		 2
#define EPH_YELLOW		 3
#define EPH_BLUE		 4
#define EPH_MAGENTA		 5
#define EPH_CYAN		 6
#define EPH_WHITE		 7
#define EPH_B_BLACK		 8
#define EPH_B_RED		 9
#define EPH_B_GREEN		10
#define EPH_B_YELLOW		11
#define EPH_B_BLUE		12
#define EPH_B_MAGENTA		13
#define EPH_B_CYAN		14
#define EPH_B_WHITE		15
#define EPH_W_BLACK		16
#define EPH_W_RED		17
#define EPH_W_GREEN		18
#define EPH_W_YELLOW		19
#define EPH_W_BLUE		20
#define EPH_W_MAGENTA		21
#define EPH_W_CYAN		22
#define EPH_W_WHITE		23

extern int dos_putc(char);
extern int dos_puts(char *);
extern int dos_printf(char *fmt, ...);
extern int EPH_Hline(char, int);
extern int EPH_Vline(char, int);
extern int getchr(void);
extern int clear_keybuf(void);
extern void EPH_SetColor(short);
extern int erase_cursor(void);
extern int disp_cursor(void);
extern int clear_eol(int);
extern int clear_bottom(int, int);
extern int ClearScreen(void);
extern int moveto(int, int);
extern int RingBell(void);
extern int CursorUp(void);
#if 0		/* ********************************* */
extern int CursorDown(void);
extern int cursor_forward(void);
extern int cursor_backward(void);
#endif		/* ********************************* */
extern int push_cursor(CursPos *);
extern int pop_cursor(CursPos *);
extern int reverse_start(void);
extern int reverse_end(void);
extern int blink_start(void);
extern int blink_end(void);
extern int invisible_start(void);
extern int bold_start(void);
extern int bold_end(void);
extern int halfbright_start(void);
extern int halfbright_end(void);
extern int modify_end(void);
extern int underline_start(void);
extern int underline_end(void);
extern int standout_start(void);
extern int standout_end(void);
extern int ins_line(void);
extern int del_line(void);
extern int lower_left(void);
extern int scroll_up(int, int);
extern int scroll_down(int, int);
extern int eph_term_init(void);
extern int eph_term_deinit(void);
extern void frame_box(int, int, int, int);
extern int EPH_getstr(int, int, char *, int);
extern int EPH_getnum(int, int, char *, int);
extern int EPH_getdecimal(int, int, char *, int);
extern int EPH_getkey(void);

/* defined eph_more.c */
extern int more_cmd(int);

#endif	/* _EPHCURS_DEF_ */

/* *****************  end of ephcurs.h  **************** */
