# *********************************************************
#
#       makefile for EPH lib - astro ( VisualSudio )
#
#       Mar.  6, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

# DEBUG		= 1

!ifdef DEBUG
DEBUG_CFLAG	= /DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAG	= /DUSE_CURL /DCURL_STATICLIB
CURL_INC        = /I..\..\$(CURL_DIR)\include
!endif

INC_DIR		= ..\..\include
LIB_DIR		= ..\..\lib
CURS_INC	= /I..\..\pdcurses

CC		= cl
CFLAGS		= /I$(INC_DIR) $(CURS_INC) $(CURL_INC) /DENABLE_COLOR /DUSE_REGEX $(CURL_CFLAG) $(DEBUG_CFLAG) /Ox /fp:precise

OBJS		= moon_loc.obj newcomb1.obj optimum.obj orbit.obj # meteor.obj

SRCS		= moon_loc.c newcomb1.c optimum.c orbit.c # meteor.c

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#
#
$(EPH_LIB) : $(OBJS)
	lib /OUT:$(EPH_LIB) $(OBJS)

.c.obj:
	$(CC) -c $(CFLAGS) $<

clean:
	del $(OBJS)

#
#	File Dependencies
#
moon_loc.obj : $(INC_DIR)\astrolib.h
optimum.obj : $(INC_DIR)\astrolib.h
orbit.obj : $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
# meteor.obj : $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
newcomb1.obj : $(INC_DIR)\astrolib.h

#
#	End of Makefile
#
