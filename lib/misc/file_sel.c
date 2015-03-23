/*
 *  file_sel.c - MPC Orbital Element File Selector
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#ifdef USE_CURL
#include <curl/curl.h>
#endif
#ifdef USE_FETCH
#include <sys/param.h>
#include <fetch.h>
#endif

#include "eph.h"
#include "menu.h"
#include "ephcurs.h"

#define M2E_HEDAER 4

#ifdef USE_CURL
static FILE *fp_out;
#endif

/*
 *  Gloval variables
 */

/*  Field M2E_Delimer of current.mpc2eph_file  */
static const char *M2E_Delim = ";\r\n";

/*  current.mpc2eph_file File pointer */
static FILE *fp_list;

/* current.mpc2eph_file handling variables */
static long List_RecAdr[1024], List_MaxRec, List_CurrRec, List_PageTop, List_PageEnd;
static char proc_mark[1024];
static int List_BreakSW = 0, List_EofSW = 0, List_ExitSW= 0, List_QuitSW = 0;
static int List_LineCnt;

/*
 * function prototypes
 */
static int  List_m2elist(void);
static int Lst_ScrHandler(void);
static int fetch_mpc(char *);
static size_t write_data(void *, size_t, size_t, void *);
static int fetch_mpc(char *);
static int disp_err(char *);

/*
 *  Listup current.mpc2eph_file
 */
static int  List_m2elist(void)
{
  int i, rec_no;
  char List_RecBuf[1024];
  char *List_buf, *List_type, *List_url, *List_comment;

  move(M2E_HEDAER, 0);
  clrtobot();
  List_LineCnt = List_ExitSW = 0;
  fseek(fp_list,  List_RecAdr[List_CurrRec], 0);
  while(! List_ExitSW) {
    if(fgets(List_RecBuf, 256, fp_list) != NULL) {
      if(*List_RecBuf == '#') {
        List_RecAdr[List_CurrRec + List_LineCnt] = ftell(fp_list);
        continue;
        }
      List_LineCnt++;
      rec_no++;
      List_RecAdr[List_CurrRec + List_LineCnt] = ftell(fp_list);
      List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
      List_type = strtok(List_buf, M2E_Delim);
      List_url = strtok(NULL, M2E_Delim);
      List_comment = strtok(NULL, M2E_Delim);
#else
      List_type = strsep(&List_buf, M2E_Delim);
      List_url = strsep(&List_buf, M2E_Delim);
      List_comment = strsep(&List_buf, M2E_Delim);
#endif
      for(i = strlen(List_url); i > 2; i--) {
        if(strncmp(List_url+i, "/", 1) == 0) { break; }
        }
/*
      printw("[ ] %-24s %-42s %5d\n", List_url+i+1, List_comment, List_RecAdr[List_CurrRec+List_LineCnt-1]);
*/
      printw("[ ] %-24s %-42s\n", List_url+i+1, List_comment);
      } else {
      List_ExitSW = List_EofSW = 1;
      move(M2E_HEDAER + List_LineCnt, 0);
      clrtobot();
      }
    }
  List_MaxRec = List_CurrRec + List_LineCnt;
  return(0);
  }

/*
 *  File Selecor Screen Handler 
 */
static int Lst_ScrHandler(void)
{
  int i, c, rec_no;
  char List_RecBuf[1024];
  char *List_buf, *List_type, *List_url, *List_comment;

  List_LineCnt = 1;
  List_BreakSW = 0;
  move(M2E_HEDAER, 1);
  disp_cursor();
  while(!List_BreakSW) {
    c = EPH_getkey();
    switch(c) {
      case KEY_NPAGE:
        if(! List_EofSW) {
          List_CurrRec += LINES - M2E_HEDAER - 1;
          }
        break;
      case KEY_PPAGE:
        if(List_CurrRec >= LINES - M2E_HEDAER - 1) {
          List_CurrRec -= LINES - M2E_HEDAER - 1;
          List_MaxRec -= LINES - M2E_HEDAER - 1;
          } else {
          List_CurrRec = 0;
          List_MaxRec = 1;
          }
        List_EofSW = 0;
        break;
      case KEY_UP:
        if(List_LineCnt > 1) {
          List_LineCnt--;
          move(M2E_HEDAER + List_LineCnt - 1, 1);
          }
        break;
      case KEY_DOWN:
        if(List_LineCnt < List_MaxRec) {
          move(M2E_HEDAER + List_LineCnt, 1);
          List_LineCnt++;
          }
        break;
      case 0x49:              /* I : IMPORT */
      case 0x69:              /* i : import */
        fseek(fp_list, List_RecAdr[List_CurrRec+List_LineCnt-1], 0);
        while(fgets(List_RecBuf, 256, fp_list) != NULL) {
          if(*List_RecBuf == '#') {
            List_RecAdr[List_CurrRec+List_LineCnt-1] = ftell(fp_list);
            continue;
            }
          List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
          List_type = strtok(List_buf, M2E_Delim);
          List_url = strtok(NULL, M2E_Delim);
#else
          List_type = strsep(&List_buf, M2E_Delim);
          List_url = strsep(&List_buf, M2E_Delim);
#endif
          for(i = strlen(List_url); i > 2; i--) {
            if(strncmp(List_url+i, "/", 1) == 0) { break; }
            }
	  imp_mpcelm(List_type, List_url+i+1);
          break;
          }
        List_ExitSW = 0;
        List_EofSW = 0;
        List_BreakSW = 1;
        break;
#if defined(USE_FETCH) || defined(USE_CURL)
      case 0x44:              /* D : DOENLOAD */
      case 0x64:              /* d : download */
        fseek(fp_list, List_RecAdr[List_CurrRec+List_LineCnt-1], 0);
        while(fgets(List_RecBuf, 256, fp_list) != NULL) {
          if(*List_RecBuf == '#') {
            List_RecAdr[List_CurrRec+List_LineCnt-1] = ftell(fp_list);
            continue;
            }
          List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
          List_type = strtok(List_buf, M2E_Delim);
          List_url = strtok(NULL, M2E_Delim);
#else
          List_type = strsep(&List_buf, M2E_Delim);
          List_url = strsep(&List_buf, M2E_Delim);
#endif
          fetch_mpc(List_url);
          break;
          }
        List_ExitSW = 0;
        List_EofSW = 0;
        List_BreakSW = 1;
        break;
#endif
      case 0x51:              /* Q   : QUIT */
      case 0x71:              /* q   : quit */
      case 0x1b:              /* ESC : quit */
        List_ExitSW = 1;
        List_BreakSW = 1;
        List_QuitSW = 1;
        break;
      case 0x20:              /* Space          */
      case 0x0a:              /* CR : ENTER KEY */
/*
        if(proc_mark[List_CurrRec + List_LineCnt - 1] == '*') {
          proc_mark[List_CurrRec + List_LineCnt - 1] = ' ';
          } else {
          proc_mark[List_CurrRec + List_LineCnt - 1] = '*';
          }
        mvprintw(M2E_HEDAER + List_LineCnt - 1, 1, "%c", proc_mark[List_CurrRec + List_LineCnt - 1]);
        if(List_LineCnt < List_MaxRec) {
          move(M2E_HEDAER + List_LineCnt, 1);
          List_LineCnt++;
          } else {
          move(M2E_HEDAER + List_LineCnt - 1, 1);
          }
*/
        break;
      default:
        break;
      }
    }
  return(0);
  }

#if defined(USE_FETCH)
/*
 *  fetch_mpc - download mpc orbital element with libfetch
 */
static int fetch_mpc(URL)
char *URL;
{
  struct url *mpc_url;
  FILE *fp_mpc, *fp_eph;
  char flags[8] = { 0 };
  struct url_stat us;
  char buf[257], *eph_file;
  size_t  size;
  int i;

  frame_box(14, 10, 52, 4);
  for(i = strlen(URL); i > 2; i--) {
    if(strncmp(URL+i, "/", 1) == 0) { break; }
    }
  eph_file = strdup(URL+i+1);

  mpc_url = fetchParseURL (URL);
  if (NULL == mpc_url) {
    mvprintw(11,15,"%s: parse error", eph_file);
    mvprintw(12,15,"  Press any key to continue.");
    getch();
    return(0);
    }

  if (fetchStat(mpc_url, &us, flags) == 0) {
    mvprintw(12,15,"File size: %d bytes", us.size);
    }

  fp_mpc = fetchXGet (mpc_url, & us, flags);
  if (NULL == fp_mpc) {
    mvprintw(11,15,"%s: %s", eph_file, fetchLastErrString);
    mvprintw(12,15,"  Press any key to continue.");
    free (mpc_url);
    getch();
    return(0);
    }

  mvprintw(11,15,"Download %s", eph_file);
  refresh();
  fp_eph = fopen(eph_file, "wb");
  while (! feof (fp_mpc) ) {
    size = fread (buf, 1, sizeof (buf), fp_mpc);
    if (size == 0) {
       break;
       }
    fwrite (buf, 1, size, fp_eph);
    }
  mvprintw(12,15,"Download completed. Press any key to continue.");
  getch();
  fclose (fp_mpc);
  fclose (fp_eph);
  free(mpc_url);
  return(0);
  }
#endif

#ifdef USE_CURL
/*
 *  fetch_mpc - download mpc orbital element with libcurl
 */

static size_t write_data( void *buffer, size_t size, size_t nmemb, void *userp )
{
  int segsize = size * nmemb;

  fprintf(fp_out, "%s", (char *)buffer);

  return segsize;
}

static int fetch_mpc(char *URL)
{
  CURL *curl;
  CURLcode ret;
  int  wr_error;
  int i;

  wr_error = 0;

  for(i = strlen(URL) - 1; i >= 0; i--) {
    if((int) *(URL+i) == '/') {
      i++;
      break;
      }
    }
  frame_box(14, 10, 52, 4);
  curl = curl_easy_init();
  if (!curl) {
    mvprintw(11,15,"%s Not found.", URL+i);
    mvprintw(12,15,"  Press any key to continue.");
    getch();
    return;
    }

  fp_out = fopen(URL+i, "wb");

  curl_easy_setopt( curl, CURLOPT_URL, URL );

  curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)&wr_error );
  curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );

  mvprintw(11,15,"Download %s", URL+i);
  refresh();
  ret = curl_easy_perform( curl );

  curl_easy_cleanup( curl );
  mvprintw(12,15,"Download completed. Press any key to continue.");
  getch();
  fclose(fp_out);

  return 0;
}
#endif

/*
 *  Display error message
 */
static int disp_err(char *msg)
{
  frame_box(14, 10, 52, 4);
  mvprintw(11,15,"%s", msg);
  mvprintw(12,15,"  Press any to continue.");
  getchr();
  clear_bottom(14, 10);
  return(0);
  }

/*
 * capitalized guide message with bold face
 */
int capitalized_guide(int x, int y, char *msg)
{

  bold_start();
  mvaddch(y, x, *msg);
  bold_end();
  mvprintw(y, x + 1, msg + 1);
}


/*
 * file_sel - MPC Orbital Element File Selector
 */
MenuFunc file_sel()
{
  char *List_buf, *List_type, *List_url, *List_comment;
  char msg[51];
  int i, c;

  clear_bottom(0, 2);
  fp_list = fopen(current.mpc2eph_file, "rb");
  if(fp_list == NULL) {
    sprintf(msg, "%s - MPC Orbit URM List not found!", current.mpc2eph_file);
    disp_err(msg);
    return(9);
    }

  List_RecAdr[0] = ftell(fp_list);
  List_CurrRec = List_PageTop = List_LineCnt = List_QuitSW = 0;

  while(!List_QuitSW) {
    underline_start();
    mvprintw(3,  0, "Sel");
    mvprintw(3,  4, "MPC Orbital Element File");
    mvprintw(3, 29, "Comment                                   \n");
    underline_end();
    List_m2elist();
    mvprintw(1, 28, "Up/Down, ");

    capitalized_guide(37, 1, "Import, ");
#if defined(USE_FETCH) || defined(USE_CURL)
    capitalized_guide(45, 1, "Download, ESC/");
    capitalized_guide(59, 1, "Quit");
#else
    capitalized_guide(45, 1, "Quit");
#endif
    clrtoeol();
    Lst_ScrHandler();
    }

  fclose(fp_list);
  clear_bottom(0, 2);
  top_bar(TB_EPH_SUB);
  guidance(GUID_SUB);

  return(0);
  }
