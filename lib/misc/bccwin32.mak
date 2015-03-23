# *********************************************************
#
#	makefile for EPH lib - misc ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.10, 2011
#       June 18, 2012
#
# *********************************************************

INC_DIR		= ..\..\include
LIB_DIR		= ..\..\lib
INCLUDES	= -I$(INC_DIR)
CURS_INC	= -I..\..\pdcurses

CC		= bcc32

!ifdef DEBUG
DEBUG_CFLAGS	= -DDEBUG -N -v -y -ff
DEBUG_LDFLAGS	= -v
!else
DEBUG_CFLAGS	=  -O -ff
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CURL -DCURL_STATICLIB
CURL_INC	= -I..\..\$(CURL_DIR)\include
!endif


CFLAGS		= $(INCLUDES) $(CURS_INC) $(CURL_INC) -DENABLE_COLOR -DUSE_REGEX $(CURL_CFLAGS) $(DEBUG_CFLAGS)

OBJS		= lib_out.obj out_calc.obj get_fn.obj filer.obj menu_man.obj \
			regex.obj mid.obj getval.obj eph_select.obj \
			sel_dev.obj time_sys.obj fmt_gets.obj s2v_v2s.obj \
			file_sel.obj imp_mpcelm.obj edit_mpcurl.obj edit_ttutc.obj

SRCS		= lib_out.c out_calc.c get_fn.c filer.c menu_man.c \
			regex.c mid.c getval.c eph_select.c \
			sel_dev.c time_sys.c fmt_gets.c s2v_v2s.c \
			file_sel.c imp_mpcelm.c edit_mpcurl.c edit_ttutc.c

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#

$(EPH_LIB) : $(OBJS)
	echo misc
	tlib $(EPH_LIB) \
		-+lib_out.obj -+out_calc.obj -+get_fn.obj -+filer.obj -+menu_man.obj \
		-+regex.obj -+mid.obj -+getval.obj -+eph_select.obj \
		-+sel_dev.obj -+time_sys.obj -+fmt_gets.obj -+s2v_v2s.obj \
		-+file_sel.obj -+imp_mpcelm.obj -+edit_mpcurl.obj -+edit_ttutc.obj

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
edit_mpcurl.obj: $(INC_DIR)\eph.h $(INC_DIR)\menu.h
edit_ttutc.obj: $(INC_DIR)\eph.h $(INC_DIR)\menu.h

#
#	End of Makefile
#
