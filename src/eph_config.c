/*
 *      eph_config.c :
 *              configuration file I/O for EPH
 *
 *      (C)opyright 2011  all right reserved
 *          Feb.02, 2011    by S.Yamane
 *
 */

#include	<stdio.h>
#include	<string.h>
#include	"eph.h"		/* current.* & ctrl_sw.* define */
#include	"getval.h"	/* val_type etc. */

typedef struct _config {
	char	*varname;
	int	vartype;
	void	*varptr;
	char	*o_fmt;
} Config;

/*
 * configuration variables
 *    name, type, pointer to variable, output format.
 *    CAUTION: item's order is important
 *             becase some item have dependencies for each.
 */
static Config config[] = {
	/* Current */
	{ "files.comet_file",	VAL_STRING,	&current.comet_file,	"%s",	},
	{ "files.mpla_file",	VAL_STRING,	&current.mpla_file,	"%s",	},
	{ "files.w_orb_comet",	VAL_STRING,	&current.w_orb_comet,	"%s",	},
	{ "files.w_orb_mpla",	VAL_STRING,	&current.w_orb_mpla,	"%s",	},
	{ "files.w_eph_file",	VAL_STRING,	&current.w_eph_file,	"%s",	},
	{ "files.w_id_file",	VAL_STRING,	&current.w_id_file,	"%s",	},
	/* =====  Ctrl_sw  =====*/
	/* -----  total control   ----- */
	{ "system.fix_elem",	VAL_INTEGER,	&ctrl_sw.fix_elem,	"%d",	},
	{ "system.use_more_eph",VAL_INTEGER,	&ctrl_sw.use_more_eph,	"%d",	},
	{ "system.use_more_id",	VAL_INTEGER,	&ctrl_sw.use_more_id,	"%d",	},
	{ "system.scan_dir",	VAL_INTEGER,	&ctrl_sw.scan_dir,	"%d",	},
	/* -----  input control   ----- */
	{ "input.eph_mode",	VAL_INTEGER,	&ctrl_sw.eph_mode,	"%d",	},
	{ "input.eph_step_sw",	VAL_INTEGER,	&ctrl_sw.eph_step_sw,	"%d",	},
	{ "input.ent_obj",	VAL_INTEGER,	&ctrl_sw.ent_obj,	"%d",	},
	{ "input.time_system",	VAL_INTEGER,	&ctrl_sw.time_system,	"%d",	},
	{ "input.orb_el_system",VAL_INTEGER,	&ctrl_sw.orb_el_system, "%d",	},
	/* -----  output control  ----- */
	{ "output.eph_out",	VAL_INTEGER,	&ctrl_sw.eph_out,	"%d",	},
	{ "output.eph_out_mode",VAL_INTEGER,	&ctrl_sw.eph_out_mode,	"%d",	},
	{ "output.id_out",	VAL_INTEGER,	&ctrl_sw.id_out,	"%d",	},
	{ "output.id_out_mode",	VAL_INTEGER,	&ctrl_sw.id_out_mode,	"%d",	},
	{ "output.orbital_sw",	VAL_INTEGER,	&ctrl_sw.orbital_sw,	"%d",	},
	{ "output.comment_sw",	VAL_INTEGER,	&ctrl_sw.comment_sw,	"%d",	},
	{ "output.micro_sw",	VAL_INTEGER,	&ctrl_sw.micro_sw,	"%d",	},
	{ "output.B1950_OUT",	VAL_INTEGER,	&ctrl_sw.B1950_OUT,	"%d",	},
	{ "output.J2000_OUT",	VAL_INTEGER,	&ctrl_sw.J2000_OUT,	"%d",	},
	{ "output.DATE_OUT",	VAL_INTEGER,	&ctrl_sw.DATE_OUT,	"%d",	},
	{ "output.delta_sw",	VAL_INTEGER,	&ctrl_sw.delta_sw,	"%d",	},
	{ "output.radius_sw",	VAL_INTEGER,	&ctrl_sw.radius_sw,	"%d",	},
	{ "output.d_mot_sw",	VAL_INTEGER,	&ctrl_sw.d_mot_sw,	"%d",	},
	{ "output.mag1_sw",	VAL_INTEGER,	&ctrl_sw.mag1_sw,	"%d",	},
	{ "output.mag2_sw",	VAL_INTEGER,	&ctrl_sw.mag2_sw,	"%d",	},
	{ "output.vmag_sw",	VAL_INTEGER,	&ctrl_sw.vmag_sw,	"%d",	},
	{ "output.elong_sw",	VAL_INTEGER,	&ctrl_sw.elong_sw,	"%d",	},
	{ "output.height_sw",	VAL_INTEGER,	&ctrl_sw.height_sw,	"%d",	},
	{ "output.tail_sw",	VAL_INTEGER,	&ctrl_sw.tail_sw,	"%d",	},
	{ "output.phase_sw",	VAL_INTEGER,	&ctrl_sw.phase_sw,	"%d",	},
	{ "output.moon_sw",	VAL_INTEGER,	&ctrl_sw.moon_sw,	"%d",	},
	{ "output.moon_light_sw",VAL_INTEGER,	&ctrl_sw.moon_light_sw,	"%d",	},
	{ "output.date_opt_sw",	VAL_INTEGER,	&ctrl_sw.date_opt_sw,	"%d",	},
	{ "output.opt_sw",	VAL_INTEGER,	&ctrl_sw.opt_sw,	"%d",	},
	{ "output.parallax_sw",	VAL_INTEGER,	&ctrl_sw.parallax_sw,	"%d",	},
	{ "output.ut2et_sw",	VAL_INTEGER,	&ctrl_sw.ut2et_sw,	"%d",	},
	{ "output.IAUC_sw",	VAL_INTEGER,	&ctrl_sw.IAUC_sw,	"%d",	},
	{ "output.mot_pa_sw",	VAL_INTEGER,	&ctrl_sw.mot_pa_sw,	"%d",	},
	{ "output.var_T_sw",	VAL_INTEGER,	&ctrl_sw.var_T_sw,	"%d",	},
	{ "output.metcalf_sw",	VAL_INTEGER,	&ctrl_sw.metcalf_sw,	"%d",	},
	{ "output.sun2obj_sw",	VAL_INTEGER,	&ctrl_sw.sun2obj_sw,	"%d",	},
	{ "output.simple_sw",	VAL_INTEGER,	&ctrl_sw.simple_sw,	"%d",	},
	{ "output.e2t_sw",	VAL_INTEGER,	&ctrl_sw.e2t_sw,	"%d",	},
	{ "output.jd_sw",	VAL_INTEGER,	&ctrl_sw.jd_sw,		"%d",	},
	{ "output.mjd_sw",	VAL_INTEGER,	&ctrl_sw.mjd_sw,	"%d",	},
	{ "output.year_sw",	VAL_INTEGER,	&ctrl_sw.year_sw,	"%d",	},
	{ "output.month_sw",	VAL_INTEGER,	&ctrl_sw.month_sw,	"%d",	},
	{ "output.day_sw",	VAL_INTEGER,	&ctrl_sw.day_sw,	"%d",	},
	{ "output.week_sw",	VAL_INTEGER,	&ctrl_sw.week_sw,	"%d",	},
	{ "output.time_sw",	VAL_INTEGER,	&ctrl_sw.time_sw,	"%d",	},
	/* -----  Location of observatory  ----- */
	{ "location.longitude",	VAL_DEGREE,	&current.longitude,
		"% 10.5lf",
	},
	{ "location.latitude",	VAL_DEGREE,	&current.latitude,
		" % 9.5lf",
	},
	{ "location.height",	VAL_DOUBLE,	&current.height,
		"%10.0lf",
	},
	/* -----  Date range  -------- */
	{ "date.id_ut_et",	VAL_DOUBLE,	&current.id_ut_et,
		"%18.6lf",
	},
	{ "date.ut_et",		VAL_DOUBLE,	&current.ut_et,
		"%18.6lf",
	},
	{ "date.id_start_jd",	VAL_LONG_JD,	&current.id_start_jd,
		"% 05d/%02d/%02d %02d:%02d:%02d",
	},
	{ "date.start_jd",	VAL_LONG_JD,	&current.start_jd,
		"% 05d/%02d/%02d %02d:%02d:%02d",
	},
	{ "date.end_jd",	VAL_LONG_JD,	&current.end_jd,
		"% 05d/%02d/%02d %02d:%02d:%02d",
	},
	{ "date.step",		VAL_DOUBLE,	&current.step,
		"%17.5lf",
	},
	/* -----  Location of target  -------- */
	{ "target.ra",		VAL_RA	,	&current.ra,
		"%3dh %02dm %05.2lfs",
	},
	{ "target.decl",	VAL_DECL,	&current.decl,
		"%+3dd %02d' %04.1lf \"",
	},
	{ "target.range",	VAL_DECL,	&current.range,
		"%3dd %02d' %04.1lf \"",
	},
	{ "target.mag_limit",	VAL_DOUBLE,	&current.mag_limit,
		"%13.1lf",
	},
	{ "target.altitude",	VAL_DEGREE,	&current.altitude,
		"%+13.1lf",
	},
	/* -----  Parameter for Metacalf  -------- */
	{ "metcalf.metcalf_p",	VAL_DOUBLE,	&current.metcalf_p,
		"% 8.5lf",
	},
#if 0
	{ "metcalf.metcalf_j",	VAL_DOUBLE,	&current.metcalf_j,	"% 8.5lf",	},
#endif

	{ NULL,			0,		NULL,			NULL,	},
};


/* ------------------------------
 * read configuration file
 * ------------------------------ */
static int search_config(char *varnam)
/*
static int
search_config(varnam)
char	*varnam;
*/
{
	int	retval;

	for (retval = 0; config[retval].varname; retval++) {
		if (strcmp(config[retval].varname, varnam) == 0) {
			return (retval);	/* found! */
		}
	}
	return (-1);
}

int
read_conf(fname)
char *fname;
{
	FILE	*fp = fopen(fname, "rb");
	char	rbuf[1024];
	const char	*delim = "\t\r\n";	/* white spaces & line terminators */
	char	*varname, *val;
	int	cfg_idx = -1;
	int	some_error = 0;
	char	err_msg[200];

	if (fp == NULL) {
		printf("ERROR: read_conf(): '%s' open failed.\n", fname);
		return (FALSE);
	}
	while (! feof(fp)) {
		if (fgets(rbuf, sizeof(rbuf), fp) == NULL) {
			break;
		}
		varname = strtok(rbuf, delim);
		val = strtok(NULL, delim);
		if ((cfg_idx = search_config(varname)) < 0) {
			sprintf(err_msg, "ERROR: read_conf(): search_config(%s) failed.\n", varname);
			pause(err_msg);
			some_error++;
		} else {
			str2val(val, config[cfg_idx].vartype, (void *)(config[cfg_idx].varptr));
		}
	}
	fclose(fp);

	return ((some_error ? FALSE : TRUE));
}

/* ------------------------------
 * save configuration file
 * ------------------------------ */
int
save_conf(fname)
char *fname;
{
	FILE	*fp = fopen(fname, "wb+");
	char	wbuf[1024], fmt[16];
	int	i;

	if (fp == NULL) {
		return (FALSE);
	}
	for (i = 0; config[i].varname != NULL; i++) {
		fprintf(fp, "%s\t", config[i].varname);
		valsprintf(wbuf, config[i].o_fmt, config[i].vartype, config[i].varptr);
		fprintf(fp, "%s\n", wbuf);
	}
	fclose(fp);

	return (TRUE);
}

/* *****************************  end of file  **************************** */
