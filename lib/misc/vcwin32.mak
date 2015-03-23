# *********************************************************
#
#       makefile for EPH lib - Misc ( VisualSudio )
#
#       Mar.  6, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

# DEBUG = 1

!ifdef DEBUG
DEBUG_CFLAGS	= /DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAGS	= /DUSE_CURL /DCURL_STATICLIB
CURL_INC	= /I..\..\$(CURL_DIR)\include
!endif

INC_DIR		= ..\..\include
CURS_INC	= /I..\..\pdcurses
LIB_DIR		= ..\..\lib

CC		= cl
CFLAGS		= /I$(INC_DIR) $(CURS_INC) $(CURL_INC) /DENABLE_COLOR /DUSE_REGEX \
			$(CURL_CFLAGS) $(DEBUG_CFLAGS) /Ox /fp:precise

OBJS		= lib_out.obj out_calc.obj get_fn.obj filer.obj menu_man.obj \
			regex.obj mid.obj getval.obj eph_select.obj \
			sel_dev.obj time_sys.obj fmt_gets.obj s2v_v2s.obj \
			file_sel.obj imp_mpcelm.obj

SRCS		= lib_out.c out_calc.c get_fn.c filer.c menu_man.c \
			regex.c mid.c getval.c eph_select.c \
			sel_dev.c time_sys.c fmt_gets.c s2v_v2s.c \
			file_sel.c imp_mpcelm.c

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#

$(EPH_LIB) : $(OBJS)
	lib $(EPH_LIB) $(OBJS)

.c.obj:
        $(CC) -c $(CFLAGS) $<

clean:
	del $(OBJS)

#
#	File Dependencies
#
fmt_gets.obj: $(INC_DIR)\menu.h
filer.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h $(INC_DIR)\filer.h
lib_out.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h $(INC_DIR)\filer.h
getval.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\getval.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h
eph_select.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h
sel_dev.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h
menu_man.obj: $(INC_DIR)\menu.h
time_sys.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
get_fn.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h
s2v_v2s.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
out_calc.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
mid.obj: 
regex.obj: $(INC_DIR)\regex.h
file_sel.obj: $(INC_DIR)\eph.h $(INC_DIR)\menu.h
imp_mpcelm.obj: $(INC_DIR)\eph.h $(INC_DIR)\menu.h

#
#	End of Makefile
#
