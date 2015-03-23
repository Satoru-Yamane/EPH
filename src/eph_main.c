/*
 *	eph_main.c :
 *		Main for EPH
 *
 *	(C)opyright 1988-1990  all right reserved
 *		Dec.21, 1988	by S.Yamane
 *						   K.Osada
 *
 *	    Apr.16, 1989	   K.Osada
 *	    Nov.25, 1990	   K.Osada
 *
 *	$Header: EPH_MAIN.Cv  1.15  91/01/20 01:19:08  yamane  Rel $
 */

#include	<stdio.h>
#include	<fcntl.h>
#include	<math.h>
#include	<signal.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	<curses.h>

#if defined(_WIN32)
#ifdef __TURBOC__
#	include	<alloc.h>
	extern int	_stklen = 6000;
#else
#	include	<malloc.h>
#endif
#endif

#define  MAIN
#	include	"astrolib.h"
#	include	"eph.h"
#	include	"ephcurs.h"
#	include	"menu.h"
#	include	"getval.h"

#define		BEGIN_GUIDANCE_X			28

static void	tb_out(char *msg)
{
	underline_start();
	dos_puts( msg );
	underline_end();
}

void
top_bar( level )
int	level;
{
	char	*wp;
	int	dev;

	reverse_start();
	moveto( 0, 0 );
	EPH_Hline( ' ', BEGIN_GUIDANCE_X );
	reverse_end();

	moveto( 28, 0 );
	underline_start();
	if ( global_sw.exec_mode == EXEC_EPH
	  || global_sw.exec_mode == EXEC_IDENT ) {
		dev = ( global_sw.exec_mode == EXEC_EPH ) ? ctrl_sw.eph_out
												  : ctrl_sw.id_out;
		switch ( dev ) {
		case DEV_CONSOLE:	dos_puts( "Console" );	break;
		case DEV_PRINTER:	dos_puts( "Printer" );	break;
		case DEV_FILE:		dos_puts( "File   " );	break;
		}
	} else { 
		dos_printf( "ver%-4.4s", VERSION );
	}
	underline_end();

	reverse_start();
	switch ( ctrl_sw.time_system ) {
	case JST_IN:	dos_puts( "JST" );		break;
	case UT_IN:		dos_puts( "UT " );		break;
	case ET_IN:		dos_puts( "ET " );		break;
	case TT_IN:		dos_puts( "TT " );		break;
	default:		dos_puts( "???" );
	}
	reverse_end();

	if ( global_sw.el_buf_stat == EL_NOT_READY )
		reverse_start();
	else
		underline_start();

	switch ( ctrl_sw.ent_obj ) {
	case COMET_IN:		dos_puts( " Comet   " );	break;
	case ASTEROID_IN:	dos_puts( "Mi.Planet" );	break;
/*	default:		dos_puts( "UnIdentMP" );  */
	}
	if ( global_sw.el_buf_stat == EL_NOT_READY )
		reverse_end();
	else
		underline_end();

	reverse_start();
	switch ( global_sw.exec_mode ) {
	case EXEC_MAIN:
			wp = "";
			break;
	case EXEC_EPH:
			wp = orb_el_buf.name;
			break;
	case EXEC_IDENT:
	case EXEC_LIB:
			if ( ctrl_sw.ent_obj == COMET_IN )
				wp = current.comet_file;
			else
				wp = current.mpla_file;
			break;
	default:
			if ( ctrl_sw.ent_obj == COMET_IN )
				wp = current.w_orb_comet;
			else
				wp = current.w_orb_mpla;
			break;
	}
	dos_printf( "[%-31.31s] ", wp );

	moveto( 0, 1 );
	EPH_Hline( ' ', EPH_cols );

	reverse_end();

	moveto( 0, 0 );
	switch ( level ) {
	case TB_MAIN:
			tb_out( CREDIT );							break;
	case TB_EPH_SUB:
			tb_out( "EPH:Ephemeris" );					break;
	case TB_ED_ORB_EL:
			tb_out( "Edit Orbital element" );			break;
	case TB_ED_DATE:
			tb_out( "Edit Current Date" );				break;
	case TB_ED_LOCATION:
			tb_out( "Edit Current Obs. Location" );		break;
	case TB_EPH_O_CTL:
			tb_out( "Ephemeris Output control" );		break;
	case TB_EPH_CALC:
			tb_out( "Ephemeris  : CALC" );				break;
	case TB_ID_SUB:
			tb_out( "EPH:Identification" );				break;
	case TB_ED_RANGE:
			tb_out( "Edit Current Range" );				break;
	case TB_ID_CALC:
			tb_out( "Identification : CALC" );			break;
	case TB_ID_O_CTL:
			tb_out( "Identification Output ctl" );		break;
	case TB_FILER_SUB:
			tb_out( "EPH : System filer" );				break;
	case TB_FILER_DELETE:
			tb_out( "Delete File records" );			break;
	case TB_FILER_UNDELETE:
			tb_out( "Un-delete File records" );			break;
	case TB_SYS_CTL_SUB:
			tb_out( "EPH : System control" );			break;
	case TB_SYS_UT:
			tb_out( "EPH : System utility" );			break;
	case TB_LIB_SEARCH:
			tb_out( "EPH : Library search" );			break;
	case TB_LOAD_ELEMENT:
			tb_out( "Load orbital element" );			break;
	default:
			tb_out( "EPH : System error" );
			RingBell();
	}
}

void
guidance( level )
int	level;
{
	reverse_start();
	moveto( BEGIN_GUIDANCE_X, 1 );
	EPH_Hline( ' ', EPH_cols - BEGIN_GUIDANCE_X );
	reverse_end();
	moveto( BEGIN_GUIDANCE_X, 1 );

	underline_start();
	switch ( level ) {
	case GUID_MAIN:
			dos_puts( "Select(SPACE/BS) : Go(CR)" );
			break;
	case GUID_SUB:
			dos_puts( "Select(SPACE/BS) : Go(CR) : Main-Menu(ESC)" );
			break;
	case GUID_YESNO:
			dos_puts( "Select(UP/DOWN/SPACE/BS) : Go(CR) : Menu(ESC)" );
			break;
	case GUID_KEYIN:
			dos_puts( "Select(UP/DOWN) : Done(CR) : Menu(ESC)" );
			break;
	case GUID_SELECT_F:
			dos_puts( "N)ext : P)revious : Execute(CR) : Cancel(ESC)" );
			break;
	case GUID_CALC:
			dos_puts( "Cancel(ESC)" );
			break;
	case GUID_WARNING:
			break;
	case GUID_PAUSE:
			dos_puts( "Hit any key..." );
			break;
	case GUID_PAUSE_YESNO:
			dos_puts( "Yes(Y/y/CR) : No(N/n/ESCAPE)" );
			break;
	case GUID_MORE:
			dos_puts( "MORE: Line(CR) : Page(SPACE) : All(x) Q)uit" );
			break;
	case GUID_QUERY:
			dos_puts( "EDIT: >(=)value / min:max / <(=)value / regex" );
			break;
	case GUID_SELECT_C:
			dos_puts( "Sel(PgDown/PgUp/down/Up):Mark(CR/SP):Imp(I):Menu(Q)" );
			break;
	default:
			dos_puts( "Select(SPACE/BS) : Go(CR) : Upper-Menu(ESC)" );
			break;
	}
	underline_end();
}

void
error( type )
int	type;
{
	RingBell();
	standout_start();

	switch ( type ) {
	case E_NOCORE:
		dos_puts( "NOT enough memory.\n" );
		break;
	case E_FOPEN:
		dos_puts( "Cannot open file.\n" );
		break;
	case E_INIT_UT2ET:
		dos_puts( "Cannot init UT-ET table.\n" );
		break;
	default:
		dos_puts( "Unkonown FATAL error.\n" );
		break;
	}

	standout_end();

	dos_puts( "Sorry...\n" );

	disp_cursor();
	ClearScreen();
	eph_term_deinit();

	exit( 9 );
}


int
warning( msg )
char	*msg;
{
	int	ik;

	blink_start();
	moveto( 0, 1 );
	RingBell();
        if(EPH_ColorMode) {
	  EPH_SetColor(EPH_B_YELLOW);
	  }
	dos_puts( msg );
        if(EPH_ColorMode) {
#ifdef _WIN32
	  EPH_SetColor(EPH_WHITE);
#else
	  EPH_SetColor(EPH_BLACK);
#endif
	  }
	blink_end();

	guidance( GUID_WARNING );

	ik = getchr();
	if ( ik == CR )
		ik = 'y';
	if ( ik == ESCAPE )
		ik = 'n';

	reverse_start();
	moveto( 0, 1 );
	EPH_Hline( ' ', EPH_cols );
	moveto( 0, 1 );
	reverse_end();

	return ( ik );
}


int
pause( msg )
char	*msg;
{
	int		ik;

	moveto( 0, 1 );
/*	RingBell(); */

	blink_start();
	if(EPH_ColorMode) {
	  EPH_SetColor(EPH_MAGENTA);
	  reverse_start();
	  }
	dos_puts( msg );
	if(EPH_ColorMode) {
#ifdef _WIN32
	  EPH_SetColor(EPH_WHITE);
#else
	  EPH_SetColor(EPH_BLACK);
#endif
	  reverse_end();
	  }
	blink_end();

	ik = getchr();
	if ( isupper( ik ) )
		ik = tolower( ik );
	if ( ik == CR )
		ik = 'y';
	if ( ik == ESCAPE )
		ik = 'n';

	moveto( 0, 1 );
	reverse_start();
	EPH_Hline( ' ', EPH_cols );
	reverse_end();

	return ( ik );
}


static void	opening(int x, int y)
{
	ClearScreen();

	moveto( x+10, y );
	if(EPH_ColorMode) {
	  EPH_SetColor(EPH_GREEN);
	  reverse_start();
	  dos_puts( " E  P  H " );
	  reverse_end();
#ifdef _WIN32
	  EPH_SetColor(EPH_WHITE);
#else
	  EPH_SetColor(EPH_BLACK);
#endif
	  } else {
	  standout_start();
	  dos_puts( " E  P  H " );
	  standout_end();
	  }

	underline_start();
	moveto( x+8, y+2 );
	dos_puts( "Version " VERSION );
	underline_end();

	moveto( x+1, y+4 );
	dos_puts( "         Ephemeris        " );

	moveto( x, y+5 );
	dos_puts( " of Comets and Minor Planets" );

	underline_start();
	moveto( x+4, y+7 );
	dos_puts( "(C)opyright 1988-2012" );
	underline_end();

	moveto( x-7, y+9 );
	dos_puts( "   by  K.Osada & S.Yamane & M.Nishiyama" );

	moveto(x - 2, y + 11);
	dos_puts( "Special thanks to J.Sano & H.Kojima" );

}


static void  usage(void)
{
	int		skip = 25;

	fputs( "\nUsage:  >eph [-option]\n", stdout );
	fputs( "option is as follows....\n", stdout );

	fputs( "\nInput control:\n", stdout );
	fputs( "-A :  asteroid.                        ", stdout );
	fputs( "-C :  comet.(default)\n", stdout );

	fputs( "-e :  ET  system.                      ", stdout );
	fputs( "-j :  JST system.(default)\n", stdout );

	fputs( "-Z :  Orbital elements (B1950).(default)        ", stdout );
	fputs( "-z :  Orbital elements (J2000).", stdout );

	fputs( "\nOutput item selection:\n", stdout );

	fputs( "-B :  by B1950.0.(default)             ", stdout );
	fputs( "-D :  by date mean.\n", stdout );

	fputs( "-E :  elongation.                      ", stdout );
	fputs( "-H :  height & azimuth.(default)\n", stdout );

	fputs( "-J :  by J2000.0.                      ", stdout );
	fputs( "-M :  moon age.\n", stdout );

	fputs( "-P :  phase angle.                     ", stdout );
	fputs( "-S :  simple method for XYZ.\n", stdout );

	fputs( "-T :  tail & position angle.           ", stdout );
	fputs( "-U :  TT - UT (Manual).\n", stdout );

	fputs( "-V :  magnitude for minor planet [IAU1985].     ", stdout );
	fputs( "-X :  IAUC format for comet.\n", stdout );

	fputs( "-d :  true distance.(default)          ", stdout );
	fputs( "-l :  motion & phase.\n", stdout );

	fputs( "-m :  precision.                       ", stdout );
	fputs( "-o :  orbital elements.(default)\n", stdout );

	fputs( "-r :  radius vector.(default)          ", stdout );
	fputs( "-v :  daily motion.\n", stdout );

	fputs( "-1 :  total magnitude(default)         ", stdout );
	fputs( "-2 :  nuclear magnitude(default)\n", stdout );

	fputs( "-# :  constants of Metcalf method.     ", stdout );
	fputs( "-$ :  longitude & latitude seeing in sun.\n", stdout );

	fputs( "-% :  angle between Earth and tail.    ", stdout );
	fputs( "-+ :  variation for T=+1 day.\n", stdout );

	fputs( "\nOthers:\n", stdout );

	fputs( "-p :  correct parallax.                ", stdout );
	fputs( "-O :  optimized observation time.\n", stdout );

	fputs( "-@ :  version and etc.                 ", stdout );
	fputs( "\n--config <rcfile>:  Use rcfile instead of $HOME/.ephrc file.\n", stdout );
	fputs( "-? :  Display this message.\n\n", stdout );

#if defined(_WIN32)
	fputs( "Option overrides the STORED switches in EPH.SYS file.\n", stdout );
	fputs( "Environment value EPH is also available.\n", stdout );
	fputs( "You can direct the directory where EPH.SYS file exists.\n", stdout );
#else
	fputs( "Option overrides the STORED switches in $HOME/.ephrc file.\n", stdout );
	fputs( "You can direct the rc file by --config option.\n", stdout );
#endif
}

static void	parse_arg(int argc, char **argv)
{
	register int	i;
	register char	*wp;

	for ( i = 1; i < argc && argv[i][0] == '-'; i++ ) {
		wp = argv[i];
		if ( strcmp( wp, "-dumb" ) == 0 ) {
			isDumb = TRUE;
			continue;
		}
		if (strcmp(wp, "--config") == 0) {
			i++;
			if (i < argc) {
				if (strlen(argv[i]) < ENV_PATH_LEN) {
					strcpy(env_path, argv[i]);
					continue;
				}
				printf("\nconfig-file name is longer than %d.\n", ENV_PATH_LEN);
			}
			usage();
			exit (9);
		}
		while ( *(++wp) ) {
			switch ( *wp ) {
			case 'A':
					ctrl_sw.ent_obj      = ASTEROID_IN;
					ctrl_sw.time_system  = ET_IN;
					ctrl_sw.orbital_sw   = REQUEST;
					ctrl_sw.B1950_OUT    = REQUEST;
					ctrl_sw.delta_sw     = REQUEST;
					ctrl_sw.radius_sw    = REQUEST;
					ctrl_sw.vmag_sw      = REQUEST;
					ctrl_sw.elong_sw     = REQUEST;
					break;
			case 'B':
					ctrl_sw.B1950_OUT    = REQUEST;
					break;
			case 'C':
					ctrl_sw.ent_obj      = COMET_IN;
					ctrl_sw.time_system  = ET_IN;
					ctrl_sw.orbital_sw   = REQUEST;
					ctrl_sw.B1950_OUT    = REQUEST;
					ctrl_sw.delta_sw     = REQUEST;
					ctrl_sw.radius_sw    = REQUEST;
					ctrl_sw.mag1_sw      = REQUEST;
					ctrl_sw.elong_sw     = REQUEST;
					break;
			case 'D':
					ctrl_sw.DATE_OUT     = REQUEST;
					break;
			case 'E':
					ctrl_sw.elong_sw     = REQUEST;
					break;
			case 'H':
					ctrl_sw.height_sw    = REQUEST;
					break;
			case 'J':
					ctrl_sw.J2000_OUT    = REQUEST;
					break;
			case 'M':
					ctrl_sw.moon_sw      = REQUEST;
					break;
			case 'O':
					ctrl_sw.opt_sw       = REQUEST;
					break;
			case 'P':
					ctrl_sw.phase_sw     = REQUEST;
					break;
			case 'S':
					ctrl_sw.simple_sw    = REQUEST;
					break;
			case 'T':
					ctrl_sw.tail_sw      = REQUEST;
					break;
			case 'U':
					ctrl_sw.ut2et_sw     = REQUEST;
					break;
			case 'V':
					ctrl_sw.vmag_sw      = REQUEST;
					break;
			case 'X':
					ctrl_sw.IAUC_sw      = REQUEST;
					break;
			case 'Z':
					ctrl_sw.orb_el_system    = B1950_IN;
					break;
			case 'd':
					ctrl_sw.delta_sw     = REQUEST;
					break;
			case 'e':
					ctrl_sw.time_system  = ET_IN;
					break;
			case 'j':
					ctrl_sw.time_system  = JST_IN;
					break;
			case 'l':
					ctrl_sw.mot_pa_sw    = REQUEST;
					break;
			case 'm':
					ctrl_sw.micro_sw     = REQUEST;
					break;
			case 'o':
					ctrl_sw.orbital_sw   = REQUEST;
					break;
			case 'p':
					ctrl_sw.parallax_sw  = REQUEST;
					break;
			case 'r':
					ctrl_sw.radius_sw    = REQUEST;
					break;
			case 'v':
					ctrl_sw.d_mot_sw     = REQUEST;
					break;
			case 'z':
					ctrl_sw.orb_el_system	= J2000_IN;
					break;
			case '1':
					ctrl_sw.mag1_sw      = REQUEST;
					break;
			case '2':
					ctrl_sw.mag2_sw      = REQUEST;
					break;
			case '#':
					ctrl_sw.metcalf_sw   = REQUEST;
					break;
			case '$':
					ctrl_sw.sun2obj_sw   = REQUEST;
					break;
			case '%':
					ctrl_sw.e2t_sw       = REQUEST;
					break;
			case '+':
					ctrl_sw.var_T_sw     = REQUEST;
					break;
			case '@':
					printf("EPH version %s\n", VERSION);
					exit( 0 );
			case '?':
					usage();
					exit( 0 );
			default:
					RingBell();
					printf("\nUnknown option '%c'\n", *wp);
					usage();
					exit( 9 );
			}
		}
	}
}

static int	set_default(void)
{
	ctrl_sw.time_system = JST_IN;

	str2val( "2011/01/01 09:00:00", VAL_LONG_JD, &current.id_start_jd );
	current.id_ut_et = ut2et( current.id_start_jd );
	str2val( "2011/01/01 20:00:00", VAL_LONG_JD, &current.start_jd );
	current.ut_et = ut2et( current.start_jd );
	str2val( "2011/01/31 20:00:00", VAL_LONG_JD, &current.end_jd );
	current.step = 10.0;

	return ( TRUE );
}


int
clr_orb_el_buf()
{
	double	w;

	memset( (char *)&orb_el_buf, '\0', sizeof( Orbit_element ) );
	memset( (char *)&comment,    '\0', sizeof( Comment ) );
	w = 2448622.5;
	orb_el_buf.time_pass = orb_el_buf.epoch = w;
	global_sw.el_buf_stat = EL_NOT_READY;
	arry_max = 0;

	return ( 0 );
}

static MenuFunc	eph_quit()
{
	int		x = 19,	y = Y_BIAS+9;
	int		wk;

	wk = 'y';
	if ( global_sw.el_buf_stat == EL_READY ) {
		clear_bottom( x-2, y-1 );
		frame_box(x-1, y-1, 41, 3);
		moveto( x, y );
		blink_start();
		if(EPH_ColorMode) {
		  EPH_SetColor(EPH_RED);
		  reverse_start();
		  }
		dos_puts( " NOT SAVED ELEMENT !! Quit OK ? (y/n)  " );
		if(EPH_ColorMode) {
		  reverse_end();
#ifdef _WIN32
		  EPH_SetColor(EPH_WHITE);
#else
		  EPH_SetColor(EPH_BLACK);
#endif
		  }
		blink_end();
		wk = warning( "" );
		clear_bottom( x-2, y-1 );
	}
	if ( isupper( wk ) )
		wk = tolower( wk );

	return ( ( wk == 'y' ) ? menu_quit() : menu_upper() | NO_REWRITE );
}


/*
 *	menu data
 */
extern MenuFunc	eph_menu();
extern MenuFunc	id_menu();
extern MenuFunc	lib_menu();
extern MenuFunc	sys_control();
extern MenuFunc	filer();

static Menu main_menu[] = {
	{ WAIT_KEY,	'E',	"Ephemeris     ",		eph_menu,		NULL   },
	{ WAIT_KEY,	'I',	"Identification",		id_menu,		NULL   },
	{ WAIT_KEY,	'L',	"Library search",		lib_menu,		NULL   },
	{ WAIT_KEY,	'S',	"System control",		sys_control,	NULL   },
	{ WAIT_KEY,	'F',	"Filer         ",		filer,			NULL   },
	{ WAIT_KEY,	'Q',	"Quit          ",		eph_quit,		NULL   },
	{ NOT_EXEC,	'\0',	NULL,					NULL,			NULL   },
} ;


static int	ReadEnv(void)
{
	int	i,	wk,	HasEnv = 0;
	Current	tmp;
	Ctrl_sw	tmp2;
	FILE	*fp;
	char	*ev;

#if defined(_WIN32)
	wk = FN_LEN - strlen("\\" ENV_FILE);	/* Max path(dir) length */
#else
	wk = FN_LEN - strlen("/" ENV_FILE);	/* Max path(dir) length */
#endif
	if (strlen(env_path) != 0) {	/* --config has been issued */
		HasEnv = read_conf(env_path);
	}

	for(i = 0; ENV_VAL[i] != NULL && HasEnv == 0; i++) {
		memset(env_path, 0, ENV_PATH_LEN);
	  	if((ev = getenv(ENV_VAL[i])) != NULL) {
	  		strcpy(env_path, ev);
#if defined(_WIN32)
	  		strcat(env_path, "\\" ENV_FILE );
	  		if(i == 1) {
				i++;
				if((ev = getenv(ENV_VAL[i])) != NULL) {
					strcpy(env_path, ev);
					strcat(env_path, "\\" ENV_FILE );
					}
				}
#else
	  		strcat(env_path, "/" ENV_FILE );
#endif
	  		}
	  	HasEnv = read_conf(env_path);
	} 

	return ( HasEnv );
}

/*
 *	main routine
 */
main( argc, argv )
int	argc;
char	**argv;
{
	int		x = 1, 	y = 2;
	int		HasEnv;
	int		deflt,	result;
	int		wk;
	char		*envv;
	FILE		*fp;
	Ctrl_sw		Old_ctrl_sw;
	Current		Old_current;

	/*
	 *	parse command line option
	 */
	if ( argc > 1 )
		parse_arg( argc, argv );

	if (ut2et_init() != 0) {
		fprintf(stderr, "Cannot initialize UT-ET table.\n");
		exit (9);
	}

	if ( eph_term_init( ) ) {
		exit( 9 );
	}
	opening( 25, 7 );

	/*
	 *	read environment file & set default values
	 */
	if ( ! ( HasEnv = ReadEnv() ) )
		set_default();

	/*
	 *	override by command line option
	 */
	if ( argc > 1 )
		parse_arg( argc, argv );

	arry_size = 1000;		/* attempt to reserve 1000 for result cache */
	eph_result = ( Eph_result *)calloc( arry_size, sizeof( Eph_result ) );
	while ( eph_result == NULL && arry_size > 20 ) {
		arry_size -= 20;
		eph_result = ( Eph_result *)calloc( arry_size, sizeof( Eph_result ) );
	}
	if ( eph_result == NULL ) {
		error( E_NOCORE );
		/* Not reached */
	}

	erase_cursor();

	signal( SIGINT, SIG_IGN );

	arry_max  = 0;
	clr_orb_el_buf();
	Old_ctrl_sw = ctrl_sw;
	Old_current = current;

	deflt = ( HasEnv ) ? 0 : 3;		/* Ephem. / system control */
	do {
		top_bar( TB_MAIN );
		guidance( GUID_MAIN );
		result = menu_man( main_menu, x, y, MN_SELECT, deflt );
		deflt = ITEM_NO( result );
	} while ( ! ( result & QUIT ) );
	/*
	 *	¸å»ÏËö
	 */
	if ( eph_result != NULL ) {
		free( eph_result );
	}

	if ( memcmp( &ctrl_sw, &Old_ctrl_sw, sizeof(Ctrl_sw) )
	  || memcmp( &current, &Old_current, sizeof(Current) ) ) {
		guidance( GUID_PAUSE_YESNO );
		wk = pause( "Save environment ? (y/n)" );
		if ( isupper( wk ) )
			wk = tolower( wk );
		if ( wk == 'y' ) {
			char *ev = getenv("HOME");
			if (HasEnv == 0 && ev != NULL) {	/* new ~/.ephrc */
				strcpy(env_path, ev);
#if defined(_WIN32)
				strcat(env_path, "\\" ENV_FILE );
#else
				strcat(env_path, "/" ENV_FILE );
#endif
			}
			save_conf(env_path);
		}
	}

	ut2et_finish();

	disp_cursor();
	ClearScreen();
	eph_term_deinit();

	exit( 0 );
}
/* ************  end of eph_main.c  ************ */
