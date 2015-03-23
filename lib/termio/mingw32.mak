# *********************************************************
#
#       makefile for EPH lib - termio ( MinGW32 gcc )
#
#       May  11, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

INC_DIR	= ../../include
LIB_DIR	= ../../lib
CURS_INC = ../../pdcurses
CURS_LIB = ../../pdcurses/win32

CC	= gcc -pipe

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

OBJS	= eph_curses.o eph_more.o
SRCS	= eph_curses.c eph_more.c

EPH_LIB	= $(LIB_DIR)/libeph.a

#
#
#
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $<
	ar r $(EPH_LIB) $*.o
	ranlib $(EPH_LIB)

$(EPH_LIB) : $(OBJS)

clean:
	@rm -f $(OBJS)

#
#	File Dependencies
#
$(OBJS) : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/ephcurs.h

#
#	End of Makefile
#
