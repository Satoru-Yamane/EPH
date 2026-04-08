/*
 * edit_mpcurl.c - Edit MPC Orbital Element URL List
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include <memory.h>

#include "eph.h"
#include "menu.h"
#include "ephcurs.h"

#define ED_URL_HEDAER	11

/*
 * Gloval variables
 */
/* Field URL_Delimer of ED_URL_List  */
static const char *URL_Delim = ";\n";

/* MPC Orbit Element URL List File pointer */
static FILE    *fp_m2e;

/* edit_mpcurl handling variables */
static long     URL_RecAdr[1024], URL_MaxRec, URL_CurrRec;
static int      URL_BreakSW = 0, URL_EofSW = 0, URL_ExitSW = 0, URL_QuitSW = 0;
static int      URL_LineCnt;

/*
 * function prototypes
 */
static int      url_err(char *);
static int      inp_url(char *, char *, char *, char *);
static void     del_url(int);
static void     add_url(int);
static void     edit_url(int);
static int      list_mpcurl(void);
static int      scr_handler(void);
static void     disp_head(void);

/*
 * Display error message
 */
static int 
url_err(char *msg)
{
    if (EPH_ColorMode) {
        EPH_SetColor(EPH_RED);
        reverse_start();
    }
    mvprintw(3, 40, "%s", msg);
    if (EPH_ColorMode) {
        reverse_end();
#ifdef _WIN32
        EPH_SetColor(EPH_WHITE);
#else
        EPH_SetColor(EPH_BLACK);
#endif
    }
    return (0);
}

/*
 * inp_url - Input URL of MPC Orbit Element
 */
static int 
inp_url(char *Type, char *URL1, char *URL2, char *Rmks)
{
    int             i, c;
    int             eof_sw = 0, exit_sw = 0, Break_SW = 0, lparen;
    long            mpc_start, rec_no, str_CurrRec;

    mvprintw(1, 28, "Up/Down/Left/Right, Enter(Fix), ESC(Cancel)");

    /*
     * put URL
     */
    underline_start();
    mvprintw(4, 15, "%-s", Type);
    mvprintw(5, 15, "%-60.60s", URL1);
    mvprintw(6, 15, "%-60.60s", URL2);
    mvprintw(7, 15, "%-60.60s", Rmks);
    underline_end();

    /*
     * URL Input
     */
    disp_cursor();
    for (i = 0; i < 4; i++) {
        underline_start();
        if (i == 0) {
            c = EPH_getstr(4, 15, Type, 1);
        }
        if (i == 1) {
            c = EPH_getstr(5, 15, URL1, 60);
        }
        if (i == 2) {
            c = EPH_getstr(6, 15, URL2, 60);
        }
        if (i == 3) {
            c = EPH_getstr(7, 15, Rmks, 60);
        }
        underline_end();
        mvprintw(3, 40, "%30s", " ");
        if (c == 0x1b) {
            break;
        }
        if ((c == KEY_UP || c == KEY_LEFT) && i > 0) {
            i -= 2;
            continue;
        }
        if (c == KEY_UP || c == KEY_LEFT) {
            i--;
            continue;
        }
        if (i == 0 && (*Type != 'C' && *Type != 'M')) {
            url_err("Type is illegal");
            i--;
            continue;
        }
        if (i == 1 && (strlen(URL1) == 0 || *URL1 == ' ')) {
            url_err("URL is empty string");
            i--;
            continue;
        }
    }

    underline_start();
    mvprintw(4, 15, "%-s", " ");
    mvprintw(5, 15, "%-60.60s", " ");
    mvprintw(6, 15, "%-60.60s", " ");
    mvprintw(7, 15, "%-60.60s", " ");
    underline_end();
    disp_head();

    return (c);
}

/*
 * del_url - Delete URL of MPC Orbit Element
 */
static void 
del_url(int pos)
{
    int             i, j;
    char            URL_RecBuf[257];

    fseek(fp_m2e, URL_RecAdr[pos], 0);
    while (fgets(URL_RecBuf, sizeof(URL_RecBuf), fp_m2e) != NULL) {
        if (*URL_RecBuf == '#') {
            URL_RecAdr[pos] = ftell(fp_m2e);
            continue;
        }
        memcpy(URL_RecBuf + 1, URL_RecBuf, 1);
        memcpy(URL_RecBuf, "#", 1);
        fseek(fp_m2e, URL_RecAdr[pos], 0);
        fwrite(URL_RecBuf, 2, 1, fp_m2e);
        break;
    }
}

/*
 * add_url - Add URL of MPC Orbit Element
 */
static void 
add_url(int pos)
{
    int             i, j, c;
    char            Type[2], URL1[257], URL2[257], Rmks[257];

    memset(Type, 0, sizeof(Type));
    memset(URL1, 0, sizeof(URL1));
    memset(URL2, 0, sizeof(URL2));
    memset(Rmks, 0, sizeof(Rmks));

    c = inp_url(Type, URL1, URL2, Rmks);
    if (c != 0x1b && c != KEY_UP && c != KEY_LEFT) {
        fseek(fp_m2e, 0, 2);
        fprintf(fp_m2e, "%c;%-s%-s;%-s\n", *Type, URL1, URL2, Rmks);
    }
}

/*
 * edit_url - Edit URL of MPC Orbit Element
 */
static void 
edit_url(int pos)
{
    int             i, j, c;
    char            URL_RecBuf[1024];
    char           *URL_buf, *URL_type, *URL_url, *URL_rmks;
    char            Type[2], URL1[257], URL2[257], Rmks[257];

    memset(Type, 0, sizeof(Type));
    memset(URL1, 0, sizeof(URL1));
    memset(URL2, 0, sizeof(URL2));
    memset(Rmks, 0, sizeof(Rmks));
    fseek(fp_m2e, URL_RecAdr[pos], 0);
    while (fgets(URL_RecBuf, sizeof(URL_RecBuf), fp_m2e) != NULL) {
        if (*URL_RecBuf == '#') {
            continue;
        }
        URL_buf = strdup(URL_RecBuf);
#ifdef _WIN32
        URL_type = strtok(URL_buf, URL_Delim);
        URL_url = strtok(NULL, URL_Delim);
        URL_rmks = strtok(NULL, URL_Delim);
#else
        URL_type = strsep(&URL_buf, URL_Delim);
        URL_url = strsep(&URL_buf, URL_Delim);
        URL_rmks = strsep(&URL_buf, URL_Delim);
#endif
        strcpy(Type, URL_type);
        strncpy(URL1, URL_url, 60);
        if (strlen(URL_url) > 60) {
            strcpy(URL2, URL_url + 60);
        }
        strcpy(Rmks, URL_rmks);
        c = inp_url(Type, URL1, URL2, Rmks);
        if (c != 0x1b && c != KEY_UP && c != KEY_LEFT) {
            del_url(pos);
            fseek(fp_m2e, 0, 2);
            fprintf(fp_m2e, "%c;%-s%-s;%-s\n", *Type, URL1, URL2, Rmks);
        }
        break;
    }
}

/*
 * Listup ED_URL_List
 */
static int 
list_mpcurl()
{
    int             i, rec_no;
    char            URL_RecBuf[1024];
    char           *URL_buf, *URL_type, *URL_url, *URL_rmks;

    move(ED_URL_HEDAER, 0);
    clrtobot();
    URL_LineCnt = 0;

    if (fp_m2e == NULL) {
        URL_ExitSW = 1;
    } else {
        fseek(fp_m2e, URL_RecAdr[URL_CurrRec], 0);
    }
    while (!URL_ExitSW) {
        if (fgets(URL_RecBuf, sizeof(URL_RecBuf), fp_m2e) != NULL) {
            if (*URL_RecBuf == '#') {
                continue;
            }
            URL_LineCnt++;
            rec_no++;
            URL_RecAdr[URL_CurrRec + URL_LineCnt] = ftell(fp_m2e);
            URL_buf = strdup(URL_RecBuf);
#ifdef _WIN32
            URL_type = strtok(URL_buf, URL_Delim);
            URL_url = strtok(NULL, URL_Delim);
            URL_rmks = strtok(NULL, URL_Delim);
#else
            URL_type = strsep(&URL_buf, URL_Delim);
            URL_url = strsep(&URL_buf, URL_Delim);
            URL_rmks = strsep(&URL_buf, URL_Delim);
#endif
            for (i = strlen(URL_url); i > 2; i--) {
                if (strncmp(URL_url + i, "/", 1) == 0) {
                    break;
                }
            }
            printw("[ ] %-24s %-42s\n", URL_url + i + 1, URL_rmks);
        } else {
            URL_ExitSW = URL_EofSW = 1;
            move(ED_URL_HEDAER + URL_LineCnt, 0);
            clrtobot();
        }
    }
    URL_MaxRec = URL_CurrRec + URL_LineCnt;

    return (0);
}

/*
 * File Selecor Screen Handler
 */
static int 
scr_handler()
{
    int             i, c, rec_no;
    char           *URL_buf, *URL_type, *URL_url, *URL_rmks;

    move(ED_URL_HEDAER, 1);
    disp_cursor();
    URL_LineCnt = 1;
    URL_BreakSW = 0;
    while (!URL_BreakSW) {
        c = EPH_getkey();
        switch (c) {
        case KEY_NPAGE:
            if (!URL_EofSW) {
                URL_CurrRec += LINES - ED_URL_HEDAER - 1;
            }
            break;
        case KEY_PPAGE:
            if (URL_CurrRec >= LINES - ED_URL_HEDAER - 1) {
                URL_CurrRec -= LINES - ED_URL_HEDAER - 1;
                URL_MaxRec -= LINES - ED_URL_HEDAER - 1;
            } else {
                URL_CurrRec = 0;
                URL_MaxRec = 1;
            }
            URL_EofSW = 0;
            break;
        case KEY_UP:
            if (URL_LineCnt > 1) {
                URL_LineCnt--;
                move(ED_URL_HEDAER + URL_LineCnt - 1, 1);
            }
            break;
        case KEY_DOWN:
            if (URL_LineCnt < URL_MaxRec) {
                move(ED_URL_HEDAER + URL_LineCnt, 1);
                URL_LineCnt++;
            }
            break;
        case 0x41:             /* A : ADD	 */
        case 0x61:             /* a : add	 */
            add_url(URL_CurrRec + URL_LineCnt - 1);
            URL_BreakSW = 1;
            break;
        case 0x44:             /* D : DELETE	 */
        case 0x64:             /* c : delete	 */
            del_url(URL_CurrRec + URL_LineCnt - 1);
            URL_BreakSW = 1;
            break;
        case 0x45:             /* E : EDIT	 */
        case 0x65:             /* e : edit	 */
            edit_url(URL_CurrRec + URL_LineCnt - 1);
            URL_BreakSW = 1;
            break;
        case 0x51:             /* Q   : QUIT */
        case 0x71:             /* q   : quit */
        case 0x1b:             /* ESC : quit */
            URL_ExitSW = 1;
            URL_BreakSW = 1;
            URL_QuitSW = 1;
            break;
        case 0x20:             /* Space          */
        case 0x0a:             /* CR : ENTER KEY */
            break;
        default:
            break;
        }
    }

    return (0);
}

/*
 * disp headers
 */
static void 
disp_head()
{
    move(2, 0);
    clrtobot();

    mvprintw(1, 28, "Up/Down, ");
    capitalized_guide(37, 1, "Add, ");
    capitalized_guide(42, 1, "Edit, ");
    capitalized_guide(48, 1, "Delete, ESC/");
    capitalized_guide(60, 1, "Quit");
    clrtoeol();

    frame_box(2, 2, 76, 7);
    mvprintw(3, 3, "MPC Orbit Element URL");

    mvprintw(4, 5, "Type    :    (C:Comet, M:Minor Planet)");
    mvprintw(5, 5, "URL     :");
    mvprintw(7, 5, "Remarks :");
    underline_start();
    mvprintw(4, 15, " ");
    mvprintw(5, 15, "                                                            ");
    mvprintw(6, 15, "                                                            ");
    mvprintw(7, 15, "                                                            ");

    underline_start();
    mvprintw(ED_URL_HEDAER - 1, 0, "Sel");
    mvprintw(ED_URL_HEDAER - 1, 4, "MPC Orbital Element File");
    mvprintw(ED_URL_HEDAER - 1, 29, "Comment                                   \n");
    underline_end();
}

/*
 * edit_mpcurl - main routine
 */
MenuFunc 
edit_mpcurl()
{
    disp_head();

    URL_CurrRec = URL_LineCnt = 0;

    fp_m2e = fopen(current.mpc2eph_file, "rb+");
    if (fp_m2e == NULL) {
        fp_m2e = fopen(current.mpc2eph_file, "wb+");
    }
    URL_RecAdr[0] = 0;

    URL_QuitSW = 0;

    while (!URL_QuitSW) {
        URL_ExitSW = 0;
        list_mpcurl();
        scr_handler();
    }

    fclose(fp_m2e);

    clear_bottom(0, 2);
    top_bar(TB_EPH_SUB);
    guidance(GUID_SUB);

    return (0);
}
