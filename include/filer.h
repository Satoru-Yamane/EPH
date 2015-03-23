/*
 *	filer.h :
 *		header for EPH filer
 *
 *	(C)opyright 1989-1990,  all right reserved
 *		Mar. 15, 1989	 by S.Yamane
 *
 *	$Header: FILER.Hv  1.8  91/04/01 17:23:42  yamane  Rel $
 */

#ifndef	_FILER_DEF_

#define	_FILER_DEF_


#ifndef SEEK_SET
#	define	SEEK_SET	( 0 )
#endif
#ifndef SEEK_END
#	define	SEEK_END	( 2 )
#endif

#define SEEK_BEFORE_TOP		( -1 )
#define SEEK_AFTER_EOF		( -2 )
#define BAD_RECORD		( -3 )
#define SEEK_ERROR		( -4 )
#define RECSIZE_ERROR		( -5 )

/*
 *	record length
 */
#define MAX_RECSIZE		( 300 )
#define COMET_SHORT		( 166 + 2 )	/* +2 means delimiter(CR/LF) */
#define COMET_LONG		( 255 + 2 )
#define MPLA_SHORT		( 116 + 2 )
#define MPLA_LONG		( 160 + 2 )
#define MPLA_IAU		( 108 + 2 )
#define MPLA_IAU_EXT		( 194 + 2 )


typedef struct _Filer_cmd {
	int		level_code;
	int		(*mask_func)( Orbit_element );
	int		(*disp_func)( int, int, Orbit_element, Comment );
	int		(*when_CR)( FILE *, int, int, Orbit_element, Comment );
} Filer_cmd;

/*
 *	function prototypes
 */
extern FILE	*eph_fopen( int * );
extern FILE	*env_fopen( char *envname, char *fname, char *mode, char *retpath );

extern int	load_element( FILE *, int, int, Orbit_element *, Comment * );

/* masking function for scan_file() */
extern int	is_live( Orbit_element );
extern int	is_deleted( Orbit_element );

/* display function for scan_file() */
extern int	display_element( int, int, Orbit_element, Comment );

/* function when CR pressed for scan_file() */
extern int	copy_to_calc_buf( FILE *, int, int, Orbit_element, Comment );

extern MenuFunc	scan_file( Filer_cmd * );

extern MenuFunc	file_sel();

extern MenuFunc	edit_ttutc();
extern MenuFunc	edit_mpcurl();

/* defined output.c */
extern int orb_el_out(Orbit_element *, Comment *, FILE *);
extern int header1(FILE *);
extern int header2(FILE *);
extern int eph_out(int, Eph_result, Eph_result, Eph_const, FILE *);
extern int id_header(double, FILE *);
extern int id_out(FILE *, char *, char *, char *, double, double, double, double, double);
extern int oc_out(FILE *, char *, char *, char *, double, double, double, double);
extern int id_trailer(FILE *);

#endif	/* _FILER_DEF_ */

/* **************  end of filer.h  *************** */
