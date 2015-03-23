# *********************************************************
#
#       makefile for EPH lib - Termio ( VisualSudio )
#
#       Mar.  6, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

# DEBUG		= 1

!ifdef DEBUG
DEBUG_CFLAGS	= /DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAGS     = /DUSE_CURL /DCURL_STATICLIB
CURL_INC        = /I..\..\$(CURL_DIR)\include
!endif

INC_DIR		= ..\..\include
CURS_INC	= /I..\..\pdcurses
LIB_DIR		= ..\..\lib

OBJS		= eph_curses.obj eph_more.obj
SRCS		= eph_curses.c eph_more.c

CC		= cl
CFLAGS		= /I$(INC_DIR) $(CURS_INC) $(CURL_INC) /DENABLE_COLOR /DUSE_REGEX $(CURL_CFLAGS) $(DEBUG_CFLAGS) /Ox /fp:precise

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
eph_curses.obj : $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h $(INC_DIR)\ephcurs.h
eph_more.obj : $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h $(INC_DIR)\ephcurs.h

#
#	End of Makefile
#
