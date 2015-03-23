# *********************************************************
#
#	makefile for EPH lib - termio ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.11, 2011
#
# *********************************************************

INC_DIR		= ..\..\include
LIB_DIR		= ..\..\lib
INCLUDES	= -I$(INC_DIR)
CURS_INC	= -I..\..\pdcurses

OBJS		= eph_curses.obj eph_more.obj
SRCS		= eph_curses.c eph_more.c

CC		= bcc32

!ifdef DEBUG
DEBUG_CFLAGS	= -DDEBUG -N -v -y -ff
!else
DEBUG_CFLAGS	=  -O -ff
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CRL -DCURL_STATICLIB
CURL_INC	= -I..\..\$(CURL_DIR)\include
!endif

CFLAGS		= $(INCLUDES) $(CURS_INC) $(CURL_INC) -DENABLE_COLOR -DUSE_REGEX $(DEBUG_CFLAGS)

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#
$(EPH_LIB) : $(OBJS)
	echo termio build
	tlib $(EPH_LIB) -+eph_curses.obj -+eph_more.obj

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
