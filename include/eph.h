/*
 *	eph.h :
 *		header for EPH
 *
 *	(C)opyright 1988,  all right reserved
 *	   Dec. 21, 1988	by K.Osada
 *	   Nov. 25, 1990	by K.Osada
 *						by S.Yamane
 *
 *	$Header: EPH.Hv  1.13  91/04/11 13:48:16  yamane  Rel $
 */

#include	"astrolib.h"

#ifndef	_EPH_DEF_

#define	_EPH_DEF_

#define		VERSION			"1.4.9a4"
#define		CREDIT			"EPH Ver1.4 Kent/Gokuh/wasat"

#define		ENV_EPH			"EPH"		/* Environment Value */
#if defined(_WIN32)
#define		ENV_FILE		"EPH.SYS"	/* Option save file  */
#else
#define		ENV_FILE		".ephrc"	/* Option save file  */
#endif

#define		UT2ET_TBL		"ut2et_tbl.dat"

#define		FN_LEN			65		/* Length of Full-path name of data files  */
#define		FN_BASE			 8		/* Length of Basename of FIle */
#define		ENV_PATH_LEN		256		/* Length of Full-path name for configration file   */

/* -----  some error code definition ----- */
#define		E_NOCORE		( 900 )		/* Not enough memory */
#define		E_FOPEN			( 901 )		/* File open error   */
#define		E_INIT_UT2ET		( 902 )		/* UT2ET init error  */

/* -----  some internal code definition ----- */
#ifndef TRUE
#define		FALSE			( 0 )
#define		TRUE			( !(FALSE) )
#endif

#define		REQUEST			( 1 )
#define		NO_REQUEST		( 0 )

#define		EL_NOT_READY		( 0 )
#define		EL_READY		( 1 )
#define		EL_SAVED		( 9 )

#define		JST_IN			( 0 )
#define		ET_IN			( 1 )
#define		UT_IN			( 2 )
#define		TT_IN			( 3 )

#define		COMET_IN		( 0 )
#define		ASTEROID_IN		( 1 )

#define		B1950_IN		( 0 )
#define		J2000_IN		( 1 )

#define		DEV_CONSOLE		( 0 )
#define		DEV_PRINTER		( 1 )
#define		DEV_FILE		( 2 )

#define		ID_O_HA			( 0 )
#define		ID_O_OC			( 1 )

#define		EPH_O_NORMAL		( 0 )

#define		X_BIAS			( 0 )
#define		Y_BIAS			( 2 )

/* -----  execution mode  ----- */
#define		EXEC_MAIN		( 0 )
#define		EXEC_EPH		( 1 )
#define		EXEC_IDENT		( 2 )
#define		EXEC_LIB		( 3 )
#define		EXEC_FILER		( 5 )

/* -----  directive level  ------ */
#define		TB_MAIN		 	 0
#define		TB_EPH_SUB	 	 1
#define		TB_ED_ORB_EL		 2
#define		TB_ED_DATE		 3
#define		TB_ED_LOCATION		 4
#define		TB_EPH_O_CTL		 5
#define		TB_EPH_CALC		 6
#define		TB_ID_SUB		 7
#define		TB_ED_RANGE		 8
#define		TB_ID_CALC		 9
#define		TB_ID_O_CTL		10
#define		TB_FILER_SUB		11
#define		TB_FILER_DELETE		12
#define		TB_FILER_UNDELETE	13
#define		TB_SYS_CTL_SUB		14
#define		TB_SYS_UT		15
#define		TB_LIB_SEARCH		16
#define		TB_LOAD_ELEMENT		17

/* -----  guidance level  -------- */
#define		GUID_MAIN		( 0 )
#define		GUID_SUB		( 1 )
#define		GUID_YESNO		( 2 )
#define		GUID_KEYIN		( 3 )
#define		GUID_SELECT_F		( 4 )
#define		GUID_CALC		( 5 )
#define		GUID_WARNING		( 6 )
#define		GUID_PAUSE		( 7 )
#define		GUID_PAUSE_YESNO	( 8 )
#define		GUID_MORE		( 9 )
#define		GUID_QUERY		( 10 )
#define		GUID_SELECT_C		( 11 )

/*
 *	more function
 *		these definition must be in terminal functions' header
 */
#define		MORE_UNKNOWN		( 0 )
#define		MORE_1_LINE		( 1 )
#define		MORE_1_PAGE		( 2 )
#define		MORE_ALL		( 3 )
#define		MORE_QUIT		( 4 )

/*
 *	Variable type define
 */
typedef struct	_Current {
	char	comet_file[ FN_LEN ];		/* comet file name		  */
	char	mpla_file[ FN_LEN ];		/* Minor-planet file name   */
	char	w_orb_comet[ FN_LEN ];		/* Work Orb. elem. : comet  */
	char	w_orb_mpla[ FN_LEN ];		/* Work Orb. elem. : mpla   */
	char	w_eph_file[ FN_LEN ];		/* Work output : Ephemeris  */
	char	w_id_file[ FN_LEN ];		/* Work output : Ident.	 */
	char	deltaT_table[ FN_LEN ];		/* TT-UTC (deltaT) Table */
	char	mpc2eph_file[ FN_LEN ];		/* mpx2eph list file */
	/* -----  Location of observatory  ----- */
	double	longitude;
	double	latitude;
	double	height;
	/* -----  Date range  -------- */
	double	id_start_jd;
	double	id_ut_et;
	double	start_jd;
	double	ut_et;
	double	end_jd;
	double	step;
	/* -----  Location of target  -------- */
	double	ra;
	double	decl;
	double	range;
	double	mag_limit;
	double	altitude;
	/* -----  Parameter for Metacalf  -------- */
	double	metcalf_p;
	double	metcalf_j;
} Current;

typedef struct	_Ctrl_sw {
	/* -----  total control   ----- */
	int	fix_elem;			/* Always show Orbital element ?	*/
	int	use_more_eph;			/* Use MORE function in Ephemerides ?	*/
	int	use_more_id;			/* Use MORE function in Identification ?	*/
	int	scan_dir;			/* Scan direction for Working file */
	/* -----  input control   ----- */
	int	eph_mode;			/* Single / Multi date		*/
	int	eph_step_sw;			/* Day / Time step		*/
	int	ent_obj;			/* Comet / Astroid 		*/
	int	time_system;			/* JST / ET / UT / TT		*/
	int	orb_el_system;			/* Epoch of Orbital elements	*/
	/* -----  output control  ----- */
	int	eph_out;			/* Ephemeris Out to file ?	*/
	int	eph_out_mode;			/* Normal / SAE 	*/
	int	id_out;				/* Identification Out to file ? */
	int	id_out_mode;			/* h/A / O-C / SAE  */
	int	orbital_sw;			/* Print orbital elements ? 	*/
	int	comment_sw;			/* Print comment ? */
	int	micro_sw;			/* Accurate RA/Decl. ? 		*/
	int	B1950_OUT;			/* B1950 ? 			*/
	int	J2000_OUT;			/* J2000 ? 			*/
	int	DATE_OUT;			/* Date  ? 			*/
	int	delta_sw;			/* Delta required ? 		*/
	int	radius_sw;			/* Radius required ? 		*/
	int	d_mot_sw;			/* Daily motion required ? 	*/
	int	mag1_sw;			/* Mag 1 required ? 		*/
	int	mag2_sw;			/* Mag 2 required ? 		*/
	int	vmag_sw;			/* Vmag  required ? 		*/
	int	elong_sw;			/* Elongation required ? 	*/
	int	height_sw;			/* Height/Longitude/Latitude ? 	*/
	int	tail_sw;			/* Tail required ? 		*/
	int	phase_sw;			/* Phase required ? 		*/
	int	moon_sw;			/* Moon age required ? 		*/
	int	moon_light_sw;			/* Moon light required ? 		*/
	int	date_opt_sw;			/* Optimum obs.date ?		*/
	int	opt_sw;				/* Optimum obs.time ?		*/
	int	parallax_sw;			/* Correct parallax ? 		*/
	int	ut2et_sw;			/* Auto / Manual UT->ET conv. 	*/
	int	IAUC_sw;			/* IAUC type output ?		*/
	int	mot_pa_sw;			/* motion PA length		*/
	int	var_T_sw;			/* T variation 			*/
	int	metcalf_sw;			/* output with Metcalf.		*/
	int	sun2obj_sw;			/* Sun B/L from object.		*/
	int	simple_sw;			/* Short format			*/
	int	e2t_sw;				/* Angle between Earth-tail	*/
	int	jd_sw;				/* jd required ? */
	int	mjd_sw;				/* mjd required ? */
	int	year_sw;			/* year required ?	*/
	int	month_sw;			/* Style of month	*/
	int	day_sw;				/* day required ?	*/
	int	week_sw;			/* day of week required ? */
	int	time_sw;			/* time required ?	*/
}  Ctrl_sw;

typedef struct _Global_sw {
	int	exec_mode;			/* Eph.=1 / Ident.=2		*/
	int	el_buf_stat;			/* Element buffer status	*/
	int	ctrl_sw_changed;		/* control sw has been changed ?	*/
} Global_sw;

typedef struct _Equ_pos {
	double	ra;
	double	decl;
} Equ_pos;

typedef struct _Eph_result {
	double	jde;
	Xyz	pos;
	Xyz	xyz;
	double	lbr_r;
	double	delta;
	double	r_obj;
} Eph_result;

typedef struct _Output {
	Equ_pos	Eq1950;				/* B1950   R.A./Decl.	*/
	Equ_pos	Eq2000;				/* J2000   R.A./Decl.	*/
	Equ_pos	EqDate;				/* Date	R.A./Decl.	*/
	double	delta;				/* Delta		*/
	double	r_obj;				/* Radius		*/
	double	ra_d_mot;			/* daily motion R.A.	*/
	double	decl_d_mot;			/* daily motion Decl.	*/
	double	mag1;				/* mag1 for comet / Vmag for astroid	*/
	double	mag2;				/* mag2 for comet	*/
	double	elong;				/* elongation		*/
	double	pos_angle;			/* Phase		*/
	double	phase_angle;			/* Phase angle		*/
	double	tail_l;				/* tail length		*/
	double	azimuth;			/* Azimath		*/
	double	height;				/* Height		*/
	double	moon_age;			/* Moon age 		*/
} Output;

typedef struct _Comment {			/* from Orbital element file */
	int	n_obs;				/* Number of observation */
	char	arc[ 24 ];			/* Arc for orbit determination */
	int	n_perterb_pla;			/* Number of planet to calc perterb. */
	char	note[ 100 ];			/* Note */
} Comment;


/*
 *	global variables
 */
#ifdef MAIN
	Ctrl_sw	ctrl_sw = {			/* System control sw packets	*/
	/* -----  total control   ----- */
		! REQUEST,			/* Not always show orbital element */
		REQUEST,			/* Use MORE function in Ephemerides */
		REQUEST,			/* Use MORE function in Identification */
		0,				/* Scan forward */
	/* -----  input control   ----- */
		0,				/* Multi Date			*/
		0,				/* Step by day			*/
		COMET_IN,			/* Comet / Astroid		*/
		JST_IN,				/* JST / TT				*/
		J2000_IN,			/* Epoch of Orbital elements	*/
	/* -----  output control  ----- */
		DEV_CONSOLE,			/* EPH: output to console	*/
		EPH_O_NORMAL,			/* EPH: output Normal format	*/
		DEV_CONSOLE,			/* ID:  output to console	*/
		ID_O_HA,			/* ID:  output H/A format	*/

		REQUEST,			/* Print orbital elements ?	*/
		! REQUEST,			/* Print comment ?	*/
		! REQUEST,			/* Accurate RA/Decl. ?		*/
		! REQUEST,			/* B1950 ?			*/
		REQUEST,			/* J2000 ?			*/
		! REQUEST,			/* Date  ?			*/
		REQUEST,			/* Delta required ?		*/
		REQUEST,			/* Radius required ?		*/
		! REQUEST,			/* Daily motion required ?	*/
		REQUEST,			/* Mag 1 required ?		*/
		! REQUEST,			/* Mag 2 required ?		*/
		! REQUEST,			/* V/B   required ?		*/
		REQUEST,			/* Elongation required ?	*/
		REQUEST,			/* Height/Longitude/Latitude ?	*/
		! REQUEST,			/* Tail required ?		*/
		! REQUEST,			/* Phase required ?		*/
		! REQUEST,			/* Moon age required ?		*/
		! REQUEST,          		/* Moon light required ? 		*/
		! REQUEST,			/* Optimum obs.date ?		*/
		! REQUEST,			/* Optimum obs.time ?		*/
		! REQUEST,			/* Correct parallax ?		*/
		! REQUEST,			/* Auto / Manual UT->ET conv.	*/
		! REQUEST,			/* IAUC type output ?		*/
		! REQUEST,			/* motion PA length		*/
		! REQUEST,			/* T variation 			*/
		! REQUEST,			/* output with Metcalf.		*/
		! REQUEST,			/* Sun B/L from object.		*/
		! REQUEST,			/* Simple XYZ.			*/
		! REQUEST,			/* Angle between Earth-tail	*/
		! REQUEST,			/* jd required ? */
		! REQUEST,			/* mjd required ? */
		! REQUEST,			/* year	required ?	*/
		REQUEST,			/* Style of month	*/
		REQUEST,			/* day required ? */
		! REQUEST,  			/* day of week required ? */
		REQUEST,			/* time required ? */
	} ;
	Current	current = {			/* Current Input  packets	*/
		"comet.dat",			/* comet file			*/
		"mpla.dat",			/* minor planet file		*/
		"comet.dat",			/* Work Orbital elem. : comet	*/
		"mpla.dat",			/* Work Orbital elem. : mpla	*/
		"$eph_out.eph",			/* work file : Ephemeris	*/
		"$id_out.eph",			/* work file : Identification	*/
		"deltaT.tab",			/* TT-UTC (deltaT) table	*/
		"mpc2eph.lst",			/* mpc orbital elem. url list	*/
	/* -----  Location of observatory  ----- */
		DEG2RAD( 135.0 ),		/* E135.0ß			*/
		DEG2RAD( 35.0 ),		/* N+35.0ß			*/
		0.0,				/* H 0.0m		*/
	/* -----  Date range  -------- */
		/*
		 *	these values will set by set_default() in eph_main.c
		 *		to fit in with calculated value correctly.
		 */
		0.0,				/* Id  date */
		0.0,				/* UT - ET(1990): Ident */
		0.0,				/* Eph start */
		0.0,				/* UT - ET(1990): Ephem */
		0.0,				/* Eph end */
		10.0,				/* Eph step:  10 day		*/
	/* -----  Location of target  -------- */
		DEG2RAD( 0.0 * 15.0 ),		/* R.A.				*/
		DEG2RAD( 0.0 ),			/* Decl				*/
		DEG2RAD( 180.0 ),		/* Range			*/
		99.9,				/* Mag. limit			*/
		DEG2RAD( -90.00000 ),		/* Min. altitude		*/
	/* -----  Parameter for Metacalf  -------- */
		1.000,				/* metcalf p */
		0.001,				/* metcalf j */
	} ;
	Output		output;			/* Output packets 		*/
	Global_sw	global_sw = {
		0,				/* Not exec yet			*/
		EL_NOT_READY,			/* Element buffer ready ?	*/
		FALSE,				/* control sw changed ?	*/
	} ;
	Orbit_element	orb_el_buf;		/* Current Orbital element	*/
	Comment		comment;		/* Element comment		*/
	Eph_result	*eph_result;		/* Ephemeris result array	*/
	int		arry_size;		/*   "		"	 "  size	*/
	int		arry_max;		/* Max contents of elements	*/
	char		env_path[ ENV_PATH_LEN ];	/* Path for option file	*/
/*  Environment variable for M2E_List search */
#if defined(_WIN32)
	/* HOMEDRIVE must be the 2nd element and HOMEPATH must be the 3rd. */
	char *ENV_VAL[] = {"EPH", "HOMEDRIVE", "HOMEPATH", NULL };
#else
	char *ENV_VAL[] = {"EPH", "HOME", NULL };
#endif
#else
	extern Ctrl_sw		ctrl_sw;
	extern Current		current;
	extern Output		output;
	extern Global_sw	global_sw;
	extern Orbit_element	orb_el_buf;
	extern Comment		comment;
	extern Eph_result	*eph_result;
	extern int		arry_size;
	extern int		arry_max;
	extern char		env_path[];
#if defined(_WIN32)
	extern char *ENV_VAL[];
#else
	extern char *ENV_VAL[];
#endif
#endif
extern int			EPH_cols;
extern int			EPH_lines;
extern int			isDumb;
extern int			EPH_ColorMode;

/*
 *	function prototypes
 */
/* defined eph_main.c */
extern void top_bar(int);
extern void guidance(int);
extern void error(int);
extern int  warning(char *);
extern int  pause(char *);
extern int  clr_orb_el_buf(void);

/* defined eph_config.c */
extern int read_conf(char *);
extern int save_conf(char *);

extern char	*mid( char *, int, int );
extern int	ra2hms( double, int *, int *, double * );
extern double	hms2ra( int, int, double );
extern int	rad2dms( double, int *, int *, double * );
extern double	dms2rad( int, int, double );
extern double	in_pi( double );
extern double	in_pix2( double );
extern double	str2jde( char * );
extern char	*jde2str( char *, double );
extern double	str2time( char * );
extern char	*time2str( char *, double );
extern double	str2ra( char * );
extern char	*ra2str( char *, double );
extern double	str2rad( char * );
extern char	*rad2str( char *, double );
extern char	*ftoa( char *, double );
extern double	atorf( char * );
extern char	*rftoa( char *, double );
extern int	strmon2mon( char * );
extern char	*mon2strmon( int );
extern char	*day_of_week( double );

#endif	/* _EPH_DEF_ */

/* *****************  end of eph.h  **************** */
