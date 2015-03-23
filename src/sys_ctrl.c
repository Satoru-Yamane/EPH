/*
 *	sys_ctrl.c :
 *		System control Menu for EPH
 *
 *	(C)opyright 1988-1990  all right reserved
 *		Dec.21, 1988	  by S.Yamane
 *		Apr.16, 1989      by K.Osada
 *      Dec. 3, 1990      by K.Osada
 *
 *	$Header: SYS_CTRL.Cv  1.11  91/01/17 15:00:16  yamane  Rel $
 */

#include	<stdio.h>
#include	<ctype.h>

#include	"astrolib.h"
#include	"eph.h"
#include	"menu.h"
#include	"ephcurs.h"

/*
 *	menu item
 */
#define		SYS_CTRL_X0			 5
#define		SYS_CTRL_X1			34
#define		SYS_CTRL_Y0			 2

static Select	sys_para[] = {
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+1,	12,	&ctrl_sw.ent_obj,
		{ "Comet",	"Mi.Planet",	NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+2,	 8,	&ctrl_sw.time_system,
		{ "JST", "ET",	"UT",	"TT",	NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+3,	12,	&ctrl_sw.ut2et_sw,
		{ "Auto",	"Manual",	NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+4,	12, &ctrl_sw.orb_el_system,
		{ "B1950",	"J2000",	NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+5,	12,	&ctrl_sw.eph_mode,
		{ "Multi",	"Single",	NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+6,	12,	&ctrl_sw.eph_step_sw,
		{ "Day",	"Time",		NULL	}
	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+12,	12,	&ctrl_sw.scan_dir,
		{ "Forward",	"Backward",	NULL	}
	},
	{ -1,		-1,		-1,	NULL,
		{ NULL }
	},
} ;
static Fname	fname[] = {
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+8,	current.comet_file,	0x0000	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+9,	current.mpla_file,	0x0000	},

	{ SYS_CTRL_X1,	SYS_CTRL_Y0+10,	current.w_orb_comet,	0x0001	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+11,	current.w_orb_mpla,	0x0001	},

	{ SYS_CTRL_X1,	SYS_CTRL_Y0+13,	current.w_eph_file,	0x0001	},
	{ SYS_CTRL_X1,	SYS_CTRL_Y0+14,	current.w_id_file,	0x0001	},
	{ -1,		-1,		NULL,			0	},
} ;


/*
 *	function
 */
extern MenuFunc	set_Metval(void);
extern MenuFunc	put_Metval(void);

static Menu	sys_con_menu[] = {
	{ AUTOEXEC,	'\0',	"Target                  ",	eph_select,	&sys_para[ 0]  },
	{ AUTOEXEC,	'\0',	"Time system             ",	eph_select,	&sys_para[ 1]  },
	{ AUTOEXEC,	'\0',	"TT - UT conversion      ",	eph_select,	&sys_para[ 2]  },
	{ AUTOEXEC,	'\0',	"Epoch of Orbital Element",	eph_select,	&sys_para[ 3]  },
	{ AUTOEXEC,	'\0',	"Eph. Date   mode        ",	eph_select,	&sys_para[ 4]  },
	{ AUTOEXEC,	'\0',	"Eph. Step   mode        ",	eph_select,	&sys_para[ 5]  },
	{ AUTOEXEC,	'\0',	"Metcalf parameter       ",	set_Metval,	NULL  },
	{ AUTOEXEC,	'\0',	"Lib. elem. File(C)      ",	get_fname,	&fname[ 0]  },
	{ AUTOEXEC,	'\0',	"Lib. elem. File(M)      ",	get_fname,	&fname[ 1]  },

	{ AUTOEXEC,	'\0',	"EPH  elem. File(C)      ",	get_fname,	&fname[ 2]  },
	{ AUTOEXEC,	'\0',	"EPH  elem. File(M)      ",	get_fname,	&fname[ 3]  },
	{ AUTOEXEC,	'\0',	"Scan direction          ",	eph_select,	&sys_para[ 6]  },

	{ AUTOEXEC,	'\0',	"Work Eph. Out File      ",	get_fname,	&fname[ 4]  },
	{ AUTOEXEC,	'\0',	"Work IdentOut File      ",	get_fname,	&fname[ 5]  },
	{ WAIT_KEY,	'\0',	"Back to Main Menu       ",	menu_upper,	NULL },
	{ NOT_EXEC,	'\0',	NULL,				NULL,		NULL },
} ;


/*
 *	sys_control ---
 *		EPH system control menu
 */
MenuFunc
sys_control()
{
	register Select	*sp;
	register Fname	*fp;
	int		target;
	int		x= SYS_CTRL_X0,	y = Y_BIAS+SYS_CTRL_Y0+1;
	int		deflt,	result;

	result = 'y';
	if ( global_sw.el_buf_stat == EL_READY ) {
		result = warning( "Break Element buffer ? (y/n)" );
	}
	if ( result != 'y' )
		return ( 0 );

	ClearScreen();
	top_bar( TB_SYS_CTL_SUB );
	guidance( GUID_YESNO );
	frame_box(x-2, y-1, 72, 17);

	for ( sp = sys_para; sp->sw_var != NULL; sp++ ) {
		disp_val( sp );
	}
	put_Metval();
	for ( fp = fname; fp->varptr != NULL; fp++ ) {
		moveto( fp->xpos, fp->ypos+Y_BIAS );
		dos_puts( fp->varptr );
	}
	result = deflt = 0;
	target = ctrl_sw.ent_obj;	/* is changed ? */
	do {
		result = menu_man( sys_con_menu, x, y, MN_INPUT, deflt );
		deflt = ITEM_NO( result );
	} while ( !( result & UPPER_MENU || result & QUIT ) ) ;
	clear_bottom( x-2, y-1 );

	if ( target != ctrl_sw.ent_obj ) {	/* is changed ? */
		clr_orb_el_buf();
		global_sw.el_buf_stat = EL_NOT_READY;
		arry_max = 0;			/* recalculation required */
		if ( ctrl_sw.ent_obj == COMET_IN ) {
			ctrl_sw.mag1_sw = REQUEST;
			ctrl_sw.vmag_sw = NO_REQUEST;
		} else {
			ctrl_sw.mag1_sw = ctrl_sw.mag2_sw = NO_REQUEST;
			ctrl_sw.vmag_sw = REQUEST;
		}
	}

	top_bar( TB_MAIN );
	guidance( GUID_MAIN );

	return ( 0 );
}

/* **************  end of sys_ctrl.c  ************* */
