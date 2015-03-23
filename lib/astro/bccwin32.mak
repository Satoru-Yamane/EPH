# *********************************************************
#
#	makefile for EPH lib - astro ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.11, 2011
#
# *********************************************************

INC_DIR		= ..\..\include
CURS_INC	= -I..\..\pdcurses
LIB_DIR		= ..\..\lib
INCLLUDE	= -I$(INC_DIR)

CC		= bcc32

!ifdef DEBUG
DEBUG_CFLAGS	= -DDEBUG -N -v -y -ff
!else
DEBUG_CFLAGS	=  -O -ff
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CURL -DCURL_STATICLIB
CURL_INC	= -I..\..\$(CURL_DIR)\include
!endif

CFLAGS		= $(INCLLUDE) $(CURS_INC) $(CURL_INC) -DENABLE_COLOR -DUSE_REGEX $(CURL_CFLAGS) $(DEBUG_CFLAGS)

OBJS		= moon_loc.obj newcomb1.obj optimum.obj orbit.obj # meteor.obj

SRCS		= moon_loc.c newcomb1.c optimum.c orbit.c # meteor.c

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#
#
$(EPH_LIB) : $(OBJS)
	echo astro
	tlib $(EPH_LIB) -+moon_loc.obj -+newcomb1.obj -+optimum.obj -+orbit.obj

.c.obj:
	$(CC) -c $(CFLAGS) $<

clean:
	del $(OBJS)

#
#	File Dependencies
#
moon_loc.obj : moon_loc.c $(INC_DIR)\astrolib.h
optimum.obj : optimum.c $(INC_DIR)\astrolib.h
orbit.obj : orbit.c $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
# meteor.obj : meteor.c $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
newcomb1.obj : newcomb1.c $(INC_DIR)\astrolib.h

#
#	End of Makefile
#
