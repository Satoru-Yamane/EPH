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

#define M2E_VER "0.94"
#define M2E_List "mpc2eph.lst"
#define M2E_HEDAER 4

#ifdef USE_CURL
FILE *fp_out;
#endif

/*
 *  Gloval variables
 */

/*  Field M2E_ListDelimer of M2E_List  */
const char *M2E_ListDelim = ";\r\n";

/*  M2E_List File pointer */
FILE *fp_list;

/* M2E_List handling variables */
long Lsit_RecAdr[1024], List_MaxRec, List_CurrRec, List_PageTop, List_PageEnd;
char proc_mark[1024];
int List_BreakSW = 0, List_EofSW = 0, List_ExitSW= 0, List_QuitSW = 0;
int List_LineCnt;

/*
 *  Search and open M2E_List
 */
int search_ListEnv(void)
{
  int i, c;
  char env_path[1024], *env_var;
  int env_sw = 0;

  for(i = 0; ENV_VAL[i] != NULL && env_sw == 0; i++) {
    memset(env_path, 0, 1024);
    if((env_var = getenv(ENV_VAL[i])) != NULL) {
      strcpy(env_path, env_var);
#if defined(_WIN32)
      if(i == 1) {
	i++;
        if((env_var = getenv(ENV_VAL[i])) != NULL) {
	  strcat(env_path, env_var);
	  }
        }
      strcat(env_path, "\\" M2E_List );
#else
      strcat(env_path, "/" M2E_List );
#endif
      if(fp_list = fopen(env_path, "rb")) { env_sw = 1; }
      } 
    } 

  if(! env_sw) {
    strcpy(env_path, M2E_List );
    if(fp_list = fopen(env_path, "rb")) {
      env_sw = 1;
      } else {
      return(9);
      }
    }
  return(0);
  }

/*
 *  Listup M2E_List
 */
int  List_m2elist(void)
{
  int i, rec_no;
  char List_RecBuf[1024];
  char *List_buf, *List_type, *List_url, *List_comment;

  move(M2E_HEDAER, 0);
  clrtobot();
  List_LineCnt = List_ExitSW = 0;
  fseek(fp_list,  Lsit_RecAdr[List_CurrRec], 0);
  while(! List_ExitSW) {
    if(fgets(List_RecBuf, 256, fp_list) != NULL) {
      List_LineCnt++;
      rec_no++;
      Lsit_RecAdr[List_CurrRec + List_LineCnt] = ftell(fp_list);
      List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
      List_type = strtok(List_buf, M2E_ListDelim);
      List_url = strtok(NULL, M2E_ListDelim);
      List_comment = strtok(NULL, M2E_ListDelim);
#else
      List_type = strsep(&List_buf, M2E_ListDelim);
      List_url = strsep(&List_buf, M2E_ListDelim);
      List_comment = strsep(&List_buf, M2E_ListDelim);
#endif
      for(i = strlen(List_url); i > 2; i--) {
        if(strncmp(List_url+i, "/", 1) == 0) { break; }
        }
/*
      printw("[ ] %-24s %-42s %5d\n", List_url+i+1, List_comment, Lsit_RecAdr[List_CurrRec+List_LineCnt-1]);
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
int Lst_ScrHandler(void)
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
        fseek(fp_list, Lsit_RecAdr[List_CurrRec+List_LineCnt-1], 0);
        if(fgets(List_RecBuf, 256, fp_list) != NULL) {
          List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
          List_type = strtok(List_buf, M2E_ListDelim);
          List_url = strtok(NULL, M2E_ListDelim);
#else
          List_type = strsep(&List_buf, M2E_ListDelim);
          List_url = strsep(&List_buf, M2E_ListDelim);
#endif
          for(i = strlen(List_url); i > 2; i--) {
            if(strncmp(List_url+i, "/", 1) == 0) { break; }
            }
	  imp_mpcelm(List_type, List_url+i+1);
          }
        List_ExitSW = 0;
        List_EofSW = 0;
        List_BreakSW = 1;
        break;
#if defined(USE_FETCH) || defined(USE_CURL)
      case 0x44:              /* D : DOENLOAD */
      case 0x64:              /* d : download */
        fseek(fp_list, Lsit_RecAdr[List_CurrRec+List_LineCnt-1], 0);
        if(fgets(List_RecBuf, 256, fp_list) != NULL) {
          List_buf = strdup(List_RecBuf);
#if defined(_WIN32)
          List_type = strtok(List_buf, M2E_ListDelim);
          List_url = strtok(NULL, M2E_ListDelim);
#else
          List_type = strsep(&List_buf, M2E_ListDelim);
          List_url = strsep(&List_buf, M2E_ListDelim);
#endif
          fetch_mpc(List_url);
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
int fetch_mpc(URL)
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
    return;
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
    return;
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

size_t write_data( void *buffer, size_t size, size_t nmemb, void *userp )
{
  int segsize = size * nmemb;

  fprintf(fp_out, "%s", (char *)buffer);

  return segsize;
}

int fetch_mpc(char *URL)
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
 *  Display Operation Guidance
 */
int disp_guid(msg)
char *msg;
{
  mvprintw(1,28,"%s", msg);
  return(0);
  }

/*
 *  Display error message
 */
int disp_err(char *msg)
{
  frame_box(14, 10, 52, 4);
  mvprintw(11,15,"%s", msg);
  mvprintw(12,15,"  Press any to continue.");
  getchr();
  clear_bottom(14, 10);
  return(0);
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
  if(search_ListEnv()) {
    sprintf(msg, "%s - Environment File not found!", M2E_List);
    disp_err(msg);
    return(9);
    }


  Lsit_RecAdr[0] = ftell(fp_list);
  List_CurrRec = List_PageTop = List_LineCnt = List_QuitSW = 0;

  while(!List_QuitSW) {
    underline_start();
    mvprintw(3,  0, "Sel");
    mvprintw(3,  4, "MPC Orbital Element File");
    mvprintw(3, 29, "Comment                                   \n");
    underline_end();
    List_m2elist();
    mvprintw(1, 28, "Up/Down, ");
    bold_start();
    mvprintw(1, 37, "I");
    bold_end();
    mvprintw(1, 38, "mport, ");
#if defined(USE_FETCH) || defined(USE_CURL)
    bold_start();
    mvprintw(1, 45, "D");
    bold_end();
    mvprintw(1, 46, "ownload, ESC/");
    bold_start();
    mvprintw(1, 59, "Q");
    bold_end();
    mvprintw(1, 60, "uit");
#else
    bold_start();
    mvprintw(1, 45, "Q");
    bold_end();
    mvprintw(1, 46, "uit");
#endif
    clrtoeol();
    Lst_ScrHandler();
    }

  fclose(fp_list);
  clear_bottom(0, 2);
  return(0);
  }
