# *********************************************************
#
#	makefile for EPH ( Borland bcc )
#
#	May.  7, 2011	M.Nishiyama
#	Sept.12, 2011
#
# *********************************************************

INC_DIR		= ..\include
LIB_DIR		= ..\lib
INCLUDES	= -I$(INC_DIR)
CURS_INC	= -I..\pdcurses
CURS_LIB	= ..\pdcurses\win32\pdcurses.lib

CC		= bcc32

!ifdef DEBUG
DEBUG_CFLAGS	= -DDEBUG -N -v -y -ff
!else
DEBUG_CFLAGS	=  -O -ff
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CRL -DCURL_STATICLIB
CURL_INC	= -I..\$(CURL_DIR)\include
LDFLAGS		=  -DCURL_STATICLIB
CURL_LIB	= ..\$(CURL_DIR)\lib\libcurl.lib
CURL_OBJS	=  ..\$(CURL_DIR)\lib\BCC_objs\asyn-ares.obj ..\$(CURL_DIR)\lib\BCC_objs\asyn-thread.obj
!endif

CFLAGS		= $(INCLUDES) $(CURS_INC) $(CURL_INC) -DENABLE_COLOR -DUSE_REGEX $(CURL_CFLAGS) $(DEBUG_CFLAGS)

LD		= ilink32

TARGET		= ..\bin\eph.exe

SRCS		= eph_calc.c eph_id.c input.c eph_eph.c eph_main.c \
			output.c sys_ctrl.c eph_config.c query.c

OBJS		= eph_calc.obj eph_id.obj input.obj eph_eph.obj eph_main.obj \
			output.obj sys_ctrl.obj eph_config.obj query.obj

EPH_LIB		= $(LIB_DIR)\eph.lib

#
#	Final targets
#
$(TARGET) : $(OBJS) $(EPH_LIB)
	bcc32 -e..\bin\eph.exe $(LDFLAGS) $(OBJS) $(EPH_LIB) $(CURS_LIB) $(CURL_LIB) $(CURL_OBJS) cw32mti.lib

.c.obj:
	$(CC) -c $(CFLAGS) $<

clean:
	@del $(TARGET) $(OBJS) ..\bin\eph.tds

#
#	file dependencies
#
eph_calc.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
eph_id.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h \
		$(INC_DIR)\filer.h
input.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h \
		$(INC_DIR)\getval.h
query.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h \
		$(INC_DIR)\filer.h $(INC_DIR)\getval.h
eph_eph.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h \
		$(INC_DIR)\filer.h
eph_main.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h \
		$(INC_DIR)\getval.h
output.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h
sys_ctrl.obj: $(INC_DIR)\astrolib.h $(INC_DIR)\eph.h $(INC_DIR)\menu.h
eph_config.obj: $(INC_DIR)\eph.h $(INC_DIR)\getval.h

#
#	end of Makefile
#
