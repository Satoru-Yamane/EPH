/*
 *	menu.h :
 *		header file for menu-manager
 *
 *	(C)opyright 1988,  all right reserved
 *		Dec.21, 1988      by S.Yamane
 *
 *	$Header: MENU.Hv  1.8  91/01/17 14:47:44  yamane  Rel $
 */

#ifndef	_MENU_DEF_
#define	_MENU_DEF_

#ifndef _ASTROLIB_DEF_
#include "astrolib.h"
#endif

/*
 *	menu type
 */
#define  MN_SELECT		( 0 )			/* only as selector */
#define  MN_INPUT		( 1 )			/* input prompt */
#define  MN_INVISIBLE		( 2 )			/* invisible title */

/*
 *	flow control code : entry
 */
#define  NOT_EXEC		( 0 )
#define  AUTOEXEC		( 1 )
#define  WAIT_KEY		( 2 )

/*
 *	flow control code : return
 */
#define  NO_REWRITE		( 0x0800 )
#define  QUIT			( 0x1000 )
#define  UPPER_MENU		( 0x2000 )
#define  NEXT_ITEM		( 0x4000 )
#define  FORE_ITEM		( 0x8000 )

/*
 *	define key-code(internal)
 */
#define  ctrl(c)		( (c)-'@' )

#define  RIGHT_ARROW		( ctrl('D') )
#define  LEFT_ARROW		( ctrl('S') )
#define  UP_ARROW		( ctrl('E') )
#define  DOWN_ARROW		( ctrl('X') )

#define  DELETE_KEY		( ctrl('G') )
#define  INSERT_KEY		( ctrl('O') )		/* toggle */
#define  REEDIT_KEY		( ctrl('R') )
#define  CANCEL_KEY		( ctrl('C') )
#define  CLEAR_KEY		( ctrl('K') )

#define  ESCAPE			( ctrl('[') )
#define  TAB			( ctrl('I') )
#define  CR			( ctrl('M') )
#define  BS			( ctrl('H') )

#define  ITEM_NO( c )		( (c>>5) & 0x001f )


typedef	unsigned short	MenuFunc;

typedef struct MenuItem {
	int		exe_flg;
	char		cmd_key;
	char		*title;
	MenuFunc	(*mfunc)();
	void		*param;
} Menu;

typedef struct _Select {
	int		xpos;		/* relative position X		*/
	int		ypos;		/* relative position Y		*/
	int		xstep;		/* X step between strings	*/
	int		*sw_var;	/* pointer to sw variable	*/
	char		*selstr[ 10 ];	/* strings if sw-var = 0 ~ 10	*/
} Select;

typedef struct _Fname {
	int		xpos;
	int		ypos;
	char		*varptr;
	int		flg;
} Fname ;

/*
 *	function prototypes
 */
extern MenuFunc	menu_man( Menu *, int, int, int, int );
extern MenuFunc	menu_quit(void);
extern MenuFunc	menu_upper(void);
extern MenuFunc	not_supported(void);
extern void	disp_sw( Select * );
extern MenuFunc	eph_select( Select * );
extern MenuFunc	get_fname( Fname * );
extern MenuFunc	get_dname( Fname * );

/* defined query.c */
extern MenuFunc get_keystr(int);
extern int      MatchKey(Orbit_element *, Comment *);

/* defined fmt_gets.c */
extern unsigned short fmt_gets(int, int, char *, char *, int);

/* defined get_fn.c */
extern int clip_blanks(char *);

/* defined eph_select.c */
extern int disp_val(Select *item);

/* defined imp_mpcelm.c */
extern int imp_mpcelm(char *, char *);

#endif	/* _MENU_DEF_ */

/* ************  end of menu.h  ************** */
