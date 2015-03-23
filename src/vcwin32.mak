# *********************************************************
#
#	makefile for EPH ( VisualSudio )
#
#	Mar.  6, 2011	M.Nishiyama
#	Sept.10, 2011
#
# *********************************************************

# DEBUG = 1

!ifdef DEBUG
DEBUG_CFLAG	= /DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAG	= /DUSE_CURL /DCURL_STATICLIB
CURL_LIB	= ..\$(CURL_DIR)\lib\release\libcurl.lib WS2_32.lib Wldap32.Lib
!endif

INCL_DIR	= ..\include
LIB_DIR		= ..\lib
CURS_INC	= ..\pdcurses
CURS_LIB	= ..\pdcurses\win32\pdcurses.lib

SRCS		= eph_calc.c eph_id.c input.c query.c eph_eph.c \
			eph_main.c output.c sys_ctrl.c eph_config.c

OBJS		= eph_calc.obj eph_id.obj input.obj query.obj eph_eph.obj \
			eph_main.obj output.obj sys_ctrl.obj eph_config.obj

EPH_LIB		= $(LIB_DIR)\eph.lib 

LIBS		= User32.Lib AdvAPI32.Lib $(EPH_LIB) $(CURS_LIB) $(CURL_LIB)

PROGRAM		= ..\bin\eph.exe

CC		= cl
CFLAGS		= /I$(INCL_DIR) /I$(CURS_INC) /DENABLE_COLOR /DUSE_REGEX $(CURL_CFLAG) $(DEBUG_CFLAG) /Ox /fp:precise

LD		= link
LDFLAGS		= $(LIBS) $(OBJS) /OUT:$(PROGRAM)

#
#	Final targets
#
$(PROGRAM) : $(OBJS) $(EPH_LIB)
	$(LD) $(LDFLAGS)

.c.obj:
	$(CC) -c $(CFLAGS) $<

clean:
	@del $(PROGRAM) $(OBJS)

#
#	file dependencies
#
eph_calc.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h
eph_id.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h \
		$(INCL_DIR)\filer.h
input.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h \
		$(INCL_DIR)\getval.h
query.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h \
		$(INCL_DIR)\filer.h $(INCL_DIR)\getval.h
eph_eph.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h \
		$(INCL_DIR)\filer.h
eph_main.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h \
		$(INCL_DIR)\getval.h
output.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h
sys_ctrl.obj: $(INCL_DIR)\astrolib.h $(INCL_DIR)\eph.h $(INCL_DIR)\menu.h
eph_config.obj: $(INCL_DIR)\eph.h $(INCL_DIR)\getval.h

#
#	end of Makefile
#
