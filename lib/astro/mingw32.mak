# *********************************************************
#
#       makefile for EPH lib - astro ( MinGW32 gcc )
#
#       May  11, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

INC_DIR	= ../../include
LIB_DIR	= ../../lib
CURS_INC = ../../pdcurses
CURS_LIB = ../../pdcurses/win32

CC	= gcc  -pipe

ifeq ($(DEBUG), 1)
CFLAGS	= -I$(INC_DIR) -I$(CURS_INC) -g -DDEBUG
else
CFLAGS	= -I$(INC_DIR) -I$(CURS_INC) -O
endif

ifeq ($(ENABLE_COLOR), 1)
CFLAGS	+= -DENABLE_COLOR
endif

ifeq ($(ENABLE_SYMBOL), 1)
CFLAGS	+= -DENABLE_SYMBOL
endif

ifeq ($(USE_CURL), 1)
CFLAGS	+= -I../../$(CURL_DIR)/include -DUSE_CURL -DCURL_STATICLIB
endif

OBJS	= moon_loc.o newcomb1.o optimum.o orbit.o # meteor.o
SRCS	= moon_loc.c newcomb1.c optimum.c orbit.c # meteor.c

LIBRARY	= $(LIB_DIR)/libeph.a

#
#
#
.c.o:
	$(CC) -c $(CFLAGS) $<
	$(AR) r $(LIBRARY) $*.o
	ranlib $(LIBRARY)

$(LIBRARY) : $(OBJS)

clean:
	@rm -f $(OBJS)

#
#	File Dependencies
#
moon_loc.o : $(INC_DIR)/astrolib.h
newcomb1.o : $(INC_DIR)/astrolib.h
optimum.o : $(INC_DIR)/astrolib.h
orbit.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h
# meteor.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h

#
#	End of Makefile
#
