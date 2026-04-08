/*
 *  edit_ttutc.c - Edit TT-UTC (deltaT) Table
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

#define ED_dT_HEDAER	10
#define ED_dT_LEN_RMKS	31
#define ED_dT_MAX_ROWS	1024

/*
 *  Gloval variables
 */
/* TT-UTC (deltaT) Table pointer */
static FILE *fp_dT;

/* Edited TT-UTC Data */
typedef struct {
    int    st_y;
    int    st_m;
    int	   st_d;
    double dT;
    char   rmks[ED_dT_LEN_RMKS];
} dT_table_t;
static dT_table_t dT_table[ED_dT_MAX_ROWS];

/* TT - UTC (deltaT) handling Paramater */
static int dT_RecCnt;
static int dT_PageNo;
static int List_SW;
static int Break_SW;

/*
 * function prototypes
 */
static void store_dT_tab(dT_table_t *);
static void mk_title(void);
static int dT_err(char *);
static int list_deltaT(void);
static int store_deltaT(void);
static int inp_dT(dT_table_t *);
static void del_dT(int);
static void add_dT(int);
static void edit_dT(int);
static int Scr_Handler(void);
static void mk_title(void);
static int put_deltaT(void);

/*
 *  Display error message
 */
static int dT_err(char *msg)
{
    if (EPH_ColorMode) {
        EPH_SetColor(EPH_RED);
    }
    reverse_start();
    blink_start();
    mvprintw(3, 40,"%s", msg);
    blink_end();
    reverse_end();
    if (EPH_ColorMode) {
#ifdef _WIN32
        EPH_SetColor(EPH_WHITE);
#else
        EPH_SetColor(EPH_BLACK);
#endif
    }

    return (0);
}

/*
 *  Listup ED_dT_List
 */
static int list_deltaT()
{
    int i, st_rec, end_rec;

    st_rec  = (LINES - ED_dT_HEDAER) * (dT_PageNo);
    if (dT_RecCnt > (LINES - ED_dT_HEDAER) * (dT_PageNo + 1)) {
        end_rec = (LINES - ED_dT_HEDAER) * (dT_PageNo + 1);
    } else {
        end_rec = dT_RecCnt;
    }
    move(ED_dT_HEDAER, 0);
    clrtobot();
    for (i = st_rec; i < end_rec; i++) {
        printw("[ ] %04d/%02d/%02d %+8.3f  %-30s\n",
               dT_table[i].st_y, dT_table[i].st_m, dT_table[i].st_d,
               dT_table[i].dT, dT_table[i].rmks);
    }
    dT_PageNo++;

    return (0);
}

/*
 *  store_dT_tab - Store TT-UTC Data to Table
 */
static void store_dT_tab(dT_table_t *t)
{
    int i, j;

    for (i = dT_RecCnt; i > 0; i--) {
        if (t->st_y > dT_table[i - 1].st_y) {
            continue;
        }
        if (t->st_y < dT_table[i - 1].st_y) {
            break;
        }
        if (t->st_m > dT_table[i - 1].st_m) {
            continue;
        }
        if (t->st_m < dT_table[i - 1].st_m) {
            break;
        }
        if (t->st_d > dT_table[i - 1].st_d) {
            continue;
        }
        if (t->st_d < dT_table[i - 1].st_d) {
            break;
        }
    }
    for (j = dT_RecCnt - 1; j >= i; j--) {
        dT_table[j + 1] = dT_table[j];
        memset(dT_table[j].rmks, 0, ED_dT_LEN_RMKS);
    }
    dT_table[i] = *t;
}

/*
 *  Store TT-UTC (deltaT) Table
 */
static int store_deltaT()
{
    char dT_buf[257], str_St_y[5], str_St_m[3], str_St_d[3], str_dT[9], str_Rmks[ED_dT_LEN_RMKS];
    dT_table_t t;
    char *p0, *p;

    memset(str_St_y, 0, sizeof(str_St_y));
    memset(str_St_m, 0, sizeof(str_St_m));
    memset(str_St_d, 0, sizeof(str_St_d));
    memset(str_dT,   0, sizeof(str_dT));
    memset(str_Rmks, 0, sizeof(str_Rmks));
    dT_RecCnt = 0;
    while (fgets(dT_buf, sizeof(dT_buf), fp_dT) != NULL) {
        if (dT_buf[0] == '#'
         || dT_buf[0] == '\r'
         || dT_buf[0] == '\n'
         || (strlen(dT_buf) < 20)) {
            continue;
        }

        p0 = p = dT_buf;
        strncpy(str_St_y, p, sizeof(str_St_y) - 1), p += sizeof(str_St_y);
        strncpy(str_St_m, p, sizeof(str_St_m) - 1), p += sizeof(str_St_m);
        strncpy(str_St_d, p, sizeof(str_St_d) - 1), p += sizeof(str_St_d);
        strncpy(str_dT,   p, sizeof(str_dT) - 1),   p += sizeof(str_dT);
        if (strlen(dT_buf) < (p - p0)) {
            memset(str_Rmks, 0, sizeof(str_Rmks));
        } else {
            strncpy(str_Rmks, p, ED_dT_LEN_RMKS - 1);
        }
        t.st_y = atoi(str_St_y);
        t.st_m = atoi(str_St_m);
        t.st_d = atoi(str_St_d);
        t.dT   = atof(str_dT);
        strcpy(t.rmks, str_Rmks);
        store_dT_tab(&t);
        dT_RecCnt++;
    }

    return(0);
}

/*
 *  inp_dT - Input TT-UTC(deltaT)
 */
static int inp_dT(dT_table_t *t)
{
    int i, c;
    char str_St_y[5], str_St_m[3], str_St_d[3], str_dT[9];

    mvprintw(1, 28, "Up/Down/Left/Right, Enter(Fix), ESC(Cancel)");

    memset(str_St_y, 0, sizeof(str_St_y));
    memset(str_St_m, 0, sizeof(str_St_m));
    memset(str_St_d, 0, sizeof(str_St_d));
    memset(str_dT,   0, sizeof(str_dT));
/*
 *  put deltaT
 */
    underline_start();
    if (t->dT == 0) {
        mvprintw(4, 15, "%4s", " ");
        mvprintw(4, 20, "%2s", " ");
        mvprintw(4, 23, "%2s", " ");
        mvprintw(5, 15, "%8s", " ");
        mvprintw(6, 15, "%30s", " ");
    } else {
        sprintf(str_St_y, "%04d",   t->st_y);
        sprintf(str_St_m, "%02d",   t->st_m);
        sprintf(str_St_d, "%02d",   t->st_d);
        sprintf(str_dT,   "%+8.3f", t->dT);
        mvprintw(4, 15, str_St_y);
        mvprintw(4, 20, str_St_m);
        mvprintw(4, 23, str_St_d);
        mvprintw(5, 15, str_dT);
        mvprintw(6, 15, "%-30.30s", t->rmks);
    }
    underline_end();

/*
 *  Paramater Input
 */
    disp_cursor();

    for (i = 0; i < 5; i++) {
        underline_start();
        switch (i) {
        case 0:
                c = EPH_getnum(4, 15, str_St_y, sizeof(str_St_y) - 1);
                break;
        case 1:
                c = EPH_getnum(4, 20, str_St_m, sizeof(str_St_m) - 1);
                break;
        case 2:
                c = EPH_getnum(4, 23, str_St_d, sizeof(str_St_d) - 1);
                break;
        case 3:
                c = EPH_getdecimal(5, 15, str_dT, sizeof(str_dT) - 1);
                break;
        case 4:
                c = EPH_getstr(6, 15, t->rmks, sizeof(t->rmks) - 1);
                break;
        default:	/* internal error */
                break;
        }
        underline_end();
        if (c == 0x1b) {
            break;
        }

        switch (i) {
        case 0:		/* year */
                t->st_y = atoi(str_St_y);
                break;
        case 1:		/* month */
                t->st_m = atoi(str_St_m);
                if (t->st_m < 1 || t->st_m > 12) {
                    dT_err("Month is illegal");
                    i--;
                    continue;
                }
                break;
        case 2:		/* day */
                t->st_d = atoi(str_St_d);
                if (t->st_d < 1 || t->st_d > 31) {
                    dT_err("Day is illegal");
                    i--;
                    continue;
                }
                break;
        case 3:		/* delta T */
                t->dT   = atof(str_dT);
                if (t->dT < 0 || t->dT > 100) {
                    dT_err("TT-UTC(deltaT) is illegal");
                    i--;
                    continue;
                }
                break;
        case 4:		/* remarks */
                break;
        default:	/* internal error */
                break;
        }

        mvprintw(3, 40, "%30s", " ");
        if ((c == KEY_UP || c == KEY_BACKSPACE || c == KEY_LEFT) && i > 0) {
            i -= 2;
            continue;
        }
        if (c == KEY_UP || c == KEY_BACKSPACE || c == KEY_LEFT) {
            break;
        }
        if ((c == KEY_DOWN || c == 0x09 || c == KEY_RIGHT) && i < 5) {
            continue;
        }
        if ((c == KEY_DOWN || c == 0x09 || c == KEY_RIGHT)) {
            i--;
            continue;
        }
    }

    underline_start();
    mvprintw(4, 15, "%4s", " ");
    mvprintw(4, 20, "%2s", " ");
    mvprintw(4, 23, "%2s", " ");
    mvprintw(5, 15, "%8s", " ");
    mvprintw(6, 15, "%30s", " ");
    underline_end();

    mk_title();

    return(c);
}

/*
 *  del_dT - Delete TT-UTC (deltaT) Data from Table
 */
static void del_dT(int pos)
{
    int i, j;

    for (i = pos - 1; i < dT_RecCnt; i++) {
        dT_table[i] = dT_table[i + 1];
    }
    dT_RecCnt--;
}

/*
 *  add_dT - Add TT-UTC (deltaT)
 */
static void add_dT(int pos)
{
    int i, j, c;
    dT_table_t tmp_t;

    memset(&tmp_t, 0, sizeof(tmp_t));
    c = inp_dT(&tmp_t);
    if (c != 0x1b && c != KEY_UP && c != KEY_BACKSPACE && c != KEY_LEFT) {
        store_dT_tab(&tmp_t);
        dT_RecCnt++;
    }
}

/*
 *  edit_dT - Edit TT-UTC (deltaT)
 */
static void edit_dT(int pos)
{
    int i, j, c;
    dT_table_t tmp_t;

    tmp_t = dT_table[pos - 1];
    c = inp_dT(&tmp_t);
    if (c != 0x1b && c != KEY_UP && c != KEY_BACKSPACE && c != KEY_LEFT) {
        del_dT(pos);
        store_dT_tab(&tmp_t);
        dT_RecCnt++;
    }
}

/*
 *  Screen Handler 
 */
static int Scr_Handler()
{
    int i, c, rec_no, st_rec, end_rec;
    char *dT_buf, *str_St_y, *str_St_m, *str_St_d, *str_dT, *str_Rmks;

    Break_SW = 1;
    move(ED_dT_HEDAER, 1);
    disp_cursor();
  
    st_rec  = (LINES - ED_dT_HEDAER) * (dT_PageNo - 1) + 1;
    end_rec = (LINES - ED_dT_HEDAER) * (dT_PageNo);
    rec_no = 1;
    while (Break_SW) {
        c = EPH_getkey();
        switch(c) {
        case KEY_NPAGE:
            if (dT_RecCnt > end_rec) {
                Break_SW = 0;
            }
            break;
        case KEY_PPAGE:
            if (dT_PageNo > 1) {
                dT_PageNo -= 2;
                Break_SW = 0;
            }
            break;
        case KEY_UP:
            if (rec_no > 1) {
                rec_no--;
                move(ED_dT_HEDAER + rec_no - 1, 1);
            }
            break;
        case KEY_DOWN:
        case 0x0a:              /* CR : ENTER KEY */
            if (rec_no < (LINES - ED_dT_HEDAER)) {
                move(ED_dT_HEDAER + rec_no, 1);
                rec_no++;
            }
            break;
        case 0x41:              /* A : ADD */
        case 0x61:              /* a : add */
            add_dT(st_rec + rec_no - 1);
            Break_SW = 0;
            dT_PageNo--;
            break;
        case 0x44:              /* D : DELETE */
        case 0x64:              /* d : delete */
            del_dT(st_rec + rec_no - 1);
            Break_SW = 0;
            dT_PageNo--;
            break;
        case 0x45:              /* E : EDIT */
        case 0x65:              /* e : edit */
            edit_dT(st_rec + rec_no - 1);
            Break_SW = 0;
            dT_PageNo--;
            break;
        case 0x51:              /* Q   : QUIT */
        case 0x71:              /* q   : quit */
        case 0x1b:              /* ESC : quit */
            List_SW = Break_SW = 0;
            break;
        default:
            break;
        }
    }

    return(0);
}

/*
 *  Print Title
 */
static void mk_title()
{
    move(2, 0);
    clrtobot();
    frame_box(2, 2, 76, 6);

    mvprintw(1, 28, "Up/Down, ");
    capitalized_guide(37, 1, "Add, ");
    capitalized_guide(42, 1, "Edit, ");
    capitalized_guide(48, 1, "Delete, ESC/");
    capitalized_guide(60, 1, "Quit");
    clrtoeol();

    mvprintw(3, 3, "TT - UTC (deltaT)");
    mvprintw(4, 5, "Date    :     /  /    (UTC)");
    mvprintw(5, 5, "TT-UTC  :             (sec)");
    mvprintw(6, 5, "Comment :                              ");
    underline_start();
    mvprintw(4, 15, "    ");
    mvprintw(4, 20, "  ");
    mvprintw(4, 23, "  ");
    mvprintw(5, 15, "        ");
    mvprintw(6, 15, "                              ");

    move(ED_dT_HEDAER-1, 0);
    mvprintw(ED_dT_HEDAER - 1,  0, "Sel");
    mvprintw(ED_dT_HEDAER - 1,  4, "Date(UTC) ");
    mvprintw(ED_dT_HEDAER - 1, 16, "TT-UTC ");
    mvprintw(ED_dT_HEDAER - 1, 25, "Comment                       ");
    underline_end();

}

/*
 *  put_deltaT - Put TT-UTC (deltaT) Table
 */
static int put_deltaT()
{
    int i;

    for (i = 0; i < dT_RecCnt; i++) {
        fprintf(fp_dT, "%4d/%02d/%02d %+8.3f %-30.30s\n",
                dT_table[i].st_y, dT_table[i].st_m, dT_table[i].st_d,
                dT_table[i].dT, dT_table[i].rmks);
    }

    return (0);
}

/*
 * edit deltaT - main routine
 */
MenuFunc edit_ttutc()
{
    dT_RecCnt = 0;
    fp_dT = fopen(current.deltaT_table, "r");
    if (fp_dT != NULL) {
        if (!feof(fp_dT)) {
            store_deltaT();
        }
    }

    mk_title();

    dT_PageNo = 0;
    List_SW = Break_SW = 1;

    while(List_SW) {
        list_deltaT();
        Scr_Handler();
    }

    if (fp_dT != NULL) {
        fclose(fp_dT);
    }
    fp_dT = fopen(current.deltaT_table, "wb");
    put_deltaT();
    fclose(fp_dT);

    ut2et_finish();
    ut2et_init();

    clear_bottom(0, 2);
    top_bar(TB_EPH_SUB);
    guidance(GUID_SUB);
      
    return(0);
}
