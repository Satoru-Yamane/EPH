/*
 * imp_mpcelm - Import MPC orbit element
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <curses.h>

#include "eph.h"
#include "menu.h"
#include "ephcurs.h"

char *Month[]={"Jan.", "Feb.", "Mar.", "Apr.", "May", "Jun.", "Jul.", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."};

#define HEADERS	11

/*
 * Global arguments
 */

/*  Environment variable for M2E_List search */
#if defined(_WIN32)
extern char *ENV_VAL[];
#else
extern char *ENV_VAL[];
#endif

/* MPC Orbit Element File */
FILE *fp_mpc;

/* MPC Orbital Element Data */
char MPC_Data[257];			/* ‹O“¹—v‘fƒf[ƒ^	*/
char CMT_No[16];			/* œa¯•„‡             */
char CMT_Period[6];			/* ŽüŠúœa¯“o˜^”Ô†	*/
char CMT_Ty[5];				/* ‹ß“ú“_’Ê‰ßŽž(”N)   */
char CMT_Tm[3];				/* ‹ß“ú“_’Ê‰ßŽž(ŒŽ)   */
char CMT_Td[8];				/* ‹ß“ú“_’Ê‰ßŽž(“ú)   */
char CMT_q[9];				/* ‹ß“ú“_‹——£(q)        */
char CMT_e[9];				/* —£S—¦(e)		*/
char CMT_Peri[9];			/* ‹ß“ú“_ˆø”(ƒÖ)	*/
char CMT_Node[9];			/* ¸Œð“_‰©Œo(ƒ¶)	*/
char CMT_Incl[9];			/* ‹O“¹ŒXŽÎŠp(i)	*/
char CMT_EpocY[5];			/* Œ³Šú(”N)		*/
char CMT_EpocM[3];			/* Œ³Šú(ŒŽ)		*/
char CMT_EpocD[3];			/* Œ³Šú(“ú)		*/
char CMT_H1[5];				/* H1			*/
char CMT_K1[5];				/* K1			*/
char CMT_Name[31];			/* œa¯–¼               */
char CMT_Ref[15];			/* ŽQÆ MPEC No.        */
char MP_No[8];				/* “o˜^”Ô†/‰¼•„‡	*/
char MP_Name[29];			/* –¼‘O			*/
char MP_Epoch[6];			/* Œ³Šú			*/
char MP_EpocC[2];			/* Œ³Šú(¢‹I)		*/
char MP_EpocY[3];			/* Œ³Šú(”N)		*/
char MP_EpocM[2];			/* Œ³Šú(ŒŽ)		*/
char MP_EpocD[2];			/* Œ³Šú(“ú)		*/
char MP_H[6];				/* â‘ÎŒõ“x		*/
char MP_G[6];				/* Œõ“xƒpƒ‰ƒ[ƒ^ G	*/
char MP_M[10];				/* •½‹Ï‹ß“_Šp		*/
char MP_Peri[10];			/* ‹ß“ú“_ˆø”(ƒÖ)	*/
char MP_Node[10];			/* ¸Œð“_‰©Œo(ƒ¶)	*/
char MP_Incl[10];			/* ‹O“¹ŒXŽÎŠp(i)	*/
char MP_e[10];				/* —£S—¦		*/
char MP_n[12];				/* •½‹Ï“ú‰^“®—Ê		*/
char MP_a[12];				/* ‹O“¹’·”¼Œa		*/
char MP_Ref[11];			/* Note.		*/

/* Edited MPC Orbital element */
int Ty, Tm, EpocY, EpocM, EpocD;
double Td, Epoch, q, e, Peri, Node, Incl, H1, K1, H, G, M, n, a;
time_t TL_Today;
time_t TL_T;
time_t TL_Epoc;
struct tm *TM_Today;
struct tm TM_T;
struct tm *TM_Epoc;

/* MPC Orbit Element handling Paramater */
long MPC_RecAdr[1024], MPC_MaxRec;
char proc_mark[1024];

/*
 *  mpc_unpack - unpack number
 *    ex. 1,2, ... A=10,B=11,C=12,...V=31
 */
int mpc_unpack(char *pakced_no)
{
int Up_no;

  if(*pakced_no > 0x29 && *pakced_no < 0x40) {
    Up_no = atoi(pakced_no);
    } else {
    Up_no = *pakced_no - 0x37;
    }
  return(Up_no);
  }

/*
 *  Edit Comet Element
 */
void edit_cmtelm(void)
{
  int i, len_Data, len_Ref, len_Name;

  len_Ref = strlen(MPC_Data) - 160;
  memset(CMT_No,     0, 16);
  memset(CMT_Period, 0,  6);
  memset(CMT_Ty,     0,  5);
  memset(CMT_Tm,     0,  3);
  memset(CMT_Td,     0,  8);
  memset(CMT_q,      0,  9);
  memset(CMT_e,      0,  9);
  memset(CMT_Peri,   0,  9);
  memset(CMT_Node,   0,  9);
  memset(CMT_Incl,   0,  9);
  memset(CMT_EpocY,  0,  5);
  memset(CMT_EpocM,  0,  3);
  memset(CMT_EpocD,  0,  3);
  memset(CMT_H1,     0,  5);
  memset(CMT_K1,     0,  5);
  memset(CMT_Name,   0, 31);
  memset(CMT_Ref,    0, 15);
  strncpy(CMT_Ty,    MPC_Data+14,   4);	/* ‹ß“ú“_’Ê‰ßŽž(”N)	 */
  strncpy(CMT_Tm,    MPC_Data+19,   2);	/* ‹ß“ú“_’Ê‰ßŽž(ŒŽ)	 */
  strncpy(CMT_Td,    MPC_Data+22,   7);	/* ‹ß“ú“_’Ê‰ßŽž(“ú)	 */
  strncpy(CMT_q,     MPC_Data+31,   8);	/* ‹ß“ú“_‹——£(q)	 */
  strncpy(CMT_e,     MPC_Data+41,   8);	/* —£S—¦(e)		 */
  strncpy(CMT_Peri,  MPC_Data+51,   8);	/* ‹ß“ú“_ˆø”(ƒÖ)	 */
  strncpy(CMT_Node,  MPC_Data+61,   8);	/* ¸Œð“_‰©Œo(ƒ¶)	 */
  strncpy(CMT_Incl,  MPC_Data+71,   8);	/* ‹O“¹ŒXŽÎŠp(i)	 */
  strncpy(CMT_EpocY, MPC_Data+81,   4);	/* Œ³Šú(”N)		 */
  strncpy(CMT_EpocM, MPC_Data+85,   2);	/* Œ³Šú(ŒŽ)		 */
  strncpy(CMT_EpocD, MPC_Data+87,   2);	/* Œ³Šú(“ú)		 */
  strncpy(CMT_H1,    MPC_Data+91,   4);	/* H1			 */
  strncpy(CMT_K1,    MPC_Data+96,   4);	/* K1			 */
  strncpy(CMT_Name,  MPC_Data+102, 30);	/* œa¯–¼		 */
  strncpy(CMT_Ref,   MPC_Data+159, len_Ref);	/* Rference MPEC No.	 */
  Ty = atoi(CMT_Ty);
  Tm = atoi(CMT_Tm);
  Td = atof(CMT_Td);
  q = atof(CMT_q);
  e = atof(CMT_e);
  Peri = atof(CMT_Peri);
  Node = atof(CMT_Node);
  Incl = atof(CMT_Incl);
  if(strcmp(CMT_EpocY, "    ")) {
    EpocY = atoi(CMT_EpocY);
    EpocM = atoi(CMT_EpocM);
    EpocD = atof(CMT_EpocD);
    } else {
    TM_T.tm_year = Ty - 1900;
    TM_T.tm_mon  = Tm - 1;
    TM_T.tm_mday = Td;
    TM_T.tm_hour = TM_T.tm_min = TM_T.tm_sec = 0;
    TL_T = mktime(&TM_T);
    if(TL_Today > TL_T) {
      TL_Epoc = TL_T + 3600 * 24 * 20;
      } else {
      TL_Epoc = TL_T - 3600 * 24 * 20;
      }
    TM_Epoc = gmtime(&TL_Epoc);
    EpocY = TM_Epoc->tm_year + 1900;
    EpocM = TM_Epoc->tm_mon + 1;
    EpocD = TM_Epoc->tm_mday;
    }
  H1 = atof(CMT_H1);
  K1 = atof(CMT_K1);
  if(CMT_Name[0] == 'C' || CMT_Name[0] == 'P') {
    for(i = 2; i < 30 && CMT_Name[i] != '('; i++) {
      CMT_No[i-2] = CMT_Name[i];
      }
    CMT_No[i] = 0x00;
    } else {
      for(i = 0; i < 30 && CMT_Name[i] != '/'; i++) {
      CMT_Period[i] = CMT_Name[i];
      }
    }
  }

/*
 *  Edit Minor Planet element
 */
void edit_mpelm(void)
{
  int i, len_Data, len_Ref, len_Name;
  int lparen;

  len_Data = strlen(MPC_Data);
  if(len_Data < 195) {
    len_Name = len_Data - 167;
    } else {
    len_Name = 28;
    }
  memset(MP_No,	   0,  8);
  memset(MP_Name,  0, 29);
  memset(MP_Epoch, 0,  6);
  memset(MP_EpocC, 0,  2);
  memset(MP_EpocY, 0,  3);
  memset(MP_EpocM, 0,  2);
  memset(MP_EpocD, 0,  2);
  memset(MP_H,	   0,  6);
  memset(MP_G,	   0,  6);
  memset(MP_M,	   0, 10);
  memset(MP_Peri,  0, 10);
  memset(MP_Node,  0, 10);
  memset(MP_Incl,  0, 10);
  memset(MP_e,	   0, 10);
  memset(MP_n,	   0, 12);
  memset(MP_a,	   0, 12);
  memset(MP_Ref,   0, 11);
  if(MPC_Data[5] == ' ') {
    if(MPC_Data[0] >= '0' && MPC_Data[0] <= '9') {
      MP_No[0] = '0';
      MP_No[1] = '0';
      strncpy(MP_No+2,   MPC_Data,      5);     /* “o˜^”Ô†     */
      } else {
      MP_No[0] = '0';
      sprintf(MP_No+1, "%2d", MPC_Data[0] - '7');
      strncpy(MP_No+3,   MPC_Data+1,      4);   /* “o˜^”Ô†     */
      }
    } else {
    for(i = 0; i <= len_Name; i++) {
      if(MPC_Data[166+i] != ' ') {
	memset(MP_No,    0,  8);
	break;
	}
      }
    }
  lparen = 0;
  for(i = 166; i < 166 + len_Name; i++) {
    if(MPC_Data[i] == ' ') { continue; }
    if(MPC_Data[i] == '(') { lparen = 1; continue; }
    if(MPC_Data[i] == ')') { lparen = 0; continue; }
    if(lparen == 1 && MPC_Data[i] > 0x29 && MPC_Data[i] < 0x40) { continue; }
    break;
    }
  strncpy(MP_Name,  MPC_Data+i,    len_Data - i - 1);	/* –¼‘O		    */
  strncpy(MP_Epoch, MPC_Data+20,   5);			/* Œ³Šú		    */
  strncpy(MP_EpocC, MPC_Data+20,   1);			/* Œ³Šú(¢‹I)	    */
  strncpy(MP_EpocY, MPC_Data+21,   2);			/* Œ³Šú(”N)	    */
  strncpy(MP_EpocM, MPC_Data+23,   1);			/* Œ³Šú(ŒŽ)	    */
  strncpy(MP_EpocD, MPC_Data+24,   1);			/* Œ³Šú(“ú)	    */
  strncpy(MP_H,	    MPC_Data+8,    5);			/* â‘ÎŒõ“x	    */
  strncpy(MP_G,	    MPC_Data+14,   5);			/* Œõ“xƒpƒ‰ƒ[ƒ^ G */
  strncpy(MP_M,	    MPC_Data+26,   9);			/* •½‹Ï‹ß“_Šp	    */
  strncpy(MP_Peri,  MPC_Data+37,   9);			/* ‹ß“ú“_ˆø”(ƒÖ)   */
  strncpy(MP_Node,  MPC_Data+48,   9);			/* ¸Œð“_‰©Œo(ƒ¶)   */
  strncpy(MP_Incl,  MPC_Data+59,   9);			/* ‹O“¹ŒXŽÎŠp(i)    */
  strncpy(MP_e,	    MPC_Data+70,   9);			/* —£S—¦	    */
  strncpy(MP_n,	    MPC_Data+80,  11);			/* •½‹Ï“ú‰^“®—Ê	    */
  strncpy(MP_a,	    MPC_Data+92,  11);			/* ‹O“¹’·”¼Œa	    */
  strncpy(MP_Ref,   MPC_Data+107, 10);			/* Note.	    */
  H = atof(MP_H);
  M = atof(MP_M);
  G = atof(MP_G);
  EpocY = mpc_unpack(MP_EpocC) * 100;
  EpocY += atoi(MP_EpocY);
  EpocM = mpc_unpack(MP_EpocM);
  EpocD = mpc_unpack(MP_EpocD);

  e = atof(MP_e);
  n = atof(MP_n);
  a = atof(MP_a);
  Peri = atof(MP_Peri);
  Node = atof(MP_Node);
  Incl = atof(MP_Incl);
  }

/*
 *  imp_elem - import markup orbits
 */
void imp_elem(char *Orb_Type)
{
  FILE *fp_eph;
  long MPC_CurrRec;

  if((int) *Orb_Type == 'C') {
    fp_eph = fopen(current.w_orb_comet, "ab");
    } else {
    fp_eph = fopen(current.w_orb_mpla, "ab");
    }
  for(MPC_CurrRec = 0; MPC_CurrRec < MPC_MaxRec; MPC_CurrRec++) {
    if(proc_mark[MPC_CurrRec] == '*') {
      fseek(fp_mpc, MPC_RecAdr[MPC_CurrRec], 0);
      fgets(MPC_Data, 256, fp_mpc);
      if((int) *Orb_Type == 'C') {
	edit_cmtelm();
	fprintf(fp_eph, "%-5s      %04d %-5.5s%8.5f %9.7f %9.7f       %9.5f %9.5f %9.5f %04d %-5.5s%2d.0     0                           0    %-28.28s %5.2f%5.2f 0.00 0.00 %-10.10s %-14s                                          \r\n", CMT_Period, Ty, Month[Tm - 1], Td, q, e, Peri, Node, Incl, EpocY, Month[EpocM - 1], EpocD, CMT_Name, H1, K1*2.5, CMT_No, CMT_Ref);
	} else {
	edit_mpelm();
	fprintf(fp_eph, "%-7.7s%7.2f%9.5f%10.5f%10.5f%9.5f%10.7f%11.8f%11.7f%6.2f%04d%02d%02d%-10s   %-30.30s%-53s\r\n", MP_No, H, M, Peri, Node, Incl, e, n, a, G, EpocY, EpocM, EpocD, MP_Ref, MP_Name, MP_Ref);
        }
      }
    }
  fclose(fp_eph);
  }

/*
 *  imp_mpcelem - import MPC orbital element
 */
int imp_mpcelm(Orb_Type, MPC_File)
char *Orb_Type, *MPC_File;
{
  int i, c, len_Data, len_Ref, len_Name, line_cnt, curr_line;
  int eof_sw=0, exit_sw=0, break_sw=0, lparen;
  long mpc_start, rec_no, MPC_CurrRec;
  char mpec_elem[129], search_str[129], start_str[11];

/*
 *  Paramater Input
 */
  clear_bottom(0, 2);
  frame_box(2, 2, 76, 6);
  if((int) *Orb_Type == 'C') {
    mvprintw(3, 3, "MPC Comet element");
    } else {
    mvprintw(3, 3, "MPC Minor Planet element");
    }
/*
 *  Display Operation Guidance
 */
  bold_start();
  mvprintw(1, 28, "Q");
  bold_end();
  mvprintw(1, 29, "uit(ESC)");
  clrtoeol();

  memset(mpec_elem, 0, 129);
  memset(search_str, 0, 129);
  memset(start_str, 0, 11);
  mvprintw(4, 5, "File   :");
  mvprintw(5, 5, "Search :");
  mvprintw(5, 55, "(empty input is all)");
  mvprintw(6, 5, "Start  :");
  underline_start();
  mvprintw(4, 14, "%-40s", mpec_elem);
  mvprintw(5, 14, "%-40s", search_str);
  mvprintw(6, 14, "%-10s", start_str);
  mvprintw(4, 14, "%-40s", MPC_File);
  strcpy(mpec_elem, MPC_File);

/*
 *  Paramater Input
 */
get_mpec_elem:
  c = EPH_getstr(4, 14, mpec_elem, 40);
  if(c == 0x1b) {
    underline_end();
    goto mpc2eph_end;
    }
get_search_str:
  c = EPH_getstr(5, 14, search_str, 40);
  if(c == 0x1b) {
    underline_end();
    goto mpc2eph_end;
    }
  if(c == KEY_UP || c == KEY_BACKSPACE || c == KEY_LEFT) { goto get_mpec_elem; }
get_start_str:
  c = EPH_getnum(6, 14, start_str, 10);
  if(c == 0x1b) {
    underline_end();
    goto mpc2eph_end;
    }
  if(c == KEY_UP || c == KEY_BACKSPACE || c == KEY_LEFT) { goto get_search_str; }
  mpc_start = strtol(start_str, NULL, 10);
  underline_end();
  mvprintw(6, 55, "start rec=%d", mpc_start);
  fp_mpc = fopen(mpec_elem, "rb");
  if(fp_mpc == NULL) {
    pause("MPC Elem. file not found");
    move(2, 0);
    clrtobot();
    return(9);
    }

/*
 *  Orbits List
 */

  MPC_RecAdr[0] = ftell(fp_mpc);
  MPC_CurrRec = 0;
  rec_no = 0;
  MPC_MaxRec = 1;
  memset(proc_mark, ' ', 1024);
  underline_start();
  if((int) *Orb_Type == 'C') {
    mvprintw(HEADERS - 3,  0, "Sel");
    mvprintw(HEADERS - 3,  4, "Name                          ");
    mvprintw(HEADERS - 3, 35, "T               ");
    mvprintw(HEADERS - 3, 52, "q        ");
    mvprintw(HEADERS - 3, 62, "Ref.          \n");
    } else {
    mvprintw(HEADERS - 3,  0, "Sel");
    mvprintw(HEADERS - 3,  4, "No.    ");
    mvprintw(HEADERS - 3, 12, "Name                         ");
    mvprintw(HEADERS - 3, 42, "e          ");
    mvprintw(HEADERS - 3, 54, "a          \n");
    }
  underline_end();
  while(! exit_sw) {
    fseek(fp_mpc, MPC_RecAdr[MPC_CurrRec], 0);
    move(HEADERS - 2, 0);
    for(line_cnt = 0; (line_cnt <= LINES - HEADERS) && (! eof_sw); line_cnt++) {
      if(fgets(MPC_Data, 256, fp_mpc) != NULL) {
        if((strlen(MPC_Data) < 169) || ! strncmp(MPC_Data, "Des'n",5) || ! strncmp(MPC_Data, "-----",5)) {
          MPC_RecAdr[MPC_CurrRec + line_cnt] = ftell(fp_mpc);
          line_cnt--;
	  continue;
	  }
        if(strlen(search_str) > 0 && strstr(MPC_Data, search_str) == NULL) {
          MPC_RecAdr[MPC_CurrRec + line_cnt] = ftell(fp_mpc);
          line_cnt--;
	  continue;
	  }
        rec_no++;
        if(rec_no < mpc_start) {
          MPC_RecAdr[MPC_CurrRec + line_cnt] = ftell(fp_mpc);
          line_cnt--;
	  continue;
	  }
        MPC_RecAdr[MPC_CurrRec + 1 + line_cnt] = ftell(fp_mpc);
        len_Data = strlen(MPC_Data);
	if((int) *Orb_Type == 'C') {
	  if(len_Data > 173) {
            pause("File not like comet Elem.");
	    fclose(fp_mpc);
            move(2, 0);
            clrtobot();
            return(9);
            }
	  edit_cmtelm();
          printw("[%c] %-30s %04d %2d %8.5f %9.7f %-14s\n", proc_mark[MPC_CurrRec + line_cnt], CMT_Name, Ty, Tm, Td, q, CMT_Ref);
	  MPC_MaxRec++;
	} else {
	  if(len_Data < 182) {
            pause("File not like M.P. Elem.");
	    fclose(fp_mpc);
            move(2, 0);
            clrtobot();
            return(9);
            }
	  edit_mpelm();
          printw("[%c] %-7.7s %-28.28s %10.7f %11.7f\n", proc_mark[MPC_CurrRec + line_cnt], MP_No, MP_Name, e, a);
	  if(MPC_MaxRec > 1023) {
            eof_sw = 1;
	    clrtobot();
	    }
	  MPC_MaxRec++;
	}
      } else {
        eof_sw = 1;
	clrtobot();
      }
    }

/*
 *  Display Operation Guidance
 */
    mvprintw(1, 28, "Up/Down, Mark(SP/CR), ");
    bold_start();
    mvprintw(1, 50, "I");
    bold_end();
    mvprintw(1, 51, ")mport, ");
    bold_start();
    mvprintw(1, 59, "Q");
    bold_end();
    mvprintw(1, 60, "uit(ESC)");
    clrtoeol();

    move(HEADERS - 2, 1);
    curr_line = 1;
    break_sw = 0;
    while(! break_sw) {
      c = EPH_getkey();
      switch(c) {
        case KEY_NPAGE:
	  if(! eof_sw) {
	    MPC_CurrRec += LINES - HEADERS + 1;
	    }
	  break_sw = 1;
	  break;
        case KEY_PPAGE:
	  if(MPC_CurrRec >= LINES - HEADERS + 1) {
	    MPC_CurrRec -= LINES - HEADERS + 1;
	    MPC_MaxRec -= LINES - HEADERS + 1;
	    } else {
	    MPC_CurrRec = 0;
	    MPC_MaxRec = 1;
	    }
	  eof_sw = 0;
	  break_sw = 1;
	  break;
        case KEY_UP:
	  if(curr_line > 1) {
	    curr_line--;
	    move(HEADERS - 2 + curr_line - 1, 1);
	    }
	  break;
        case KEY_DOWN:
	  if(curr_line < line_cnt) {
	    move(HEADERS - 2 + curr_line, 1);
	    curr_line++;
	    }
	  break;
        case 0x20:		/* Space          */
        case 0x0a:		/* CR : ENTER KEY */
	  if(proc_mark[MPC_CurrRec + curr_line - 1] == '*') {
            proc_mark[MPC_CurrRec + curr_line - 1] = ' ';
            } else {
            proc_mark[MPC_CurrRec + curr_line - 1] = '*';
            }
          mvprintw(HEADERS - 2 + curr_line - 1, 1, "%c", proc_mark[MPC_CurrRec + curr_line - 1]);
	  if(curr_line < line_cnt) {
	    move(HEADERS - 2 + curr_line, 1);
	    curr_line++;
	    } else {
	    move(HEADERS - 2 + curr_line - 1, 1);
            }
	  break;
        case 0x49:		/* I : IMPORT */
        case 0x69:		/* i : import */
	  imp_elem(Orb_Type);
        case 0x51:		/* Q : QUIT */
        case 0x71:		/* q : quit */
	  break_sw = 1;
	  exit_sw = 1;
	  break;
        }
      }
    }

  fclose(fp_mpc);
mpc2eph_end:
  move(2, 0);
  clrtobot();
  return(0);
  }
