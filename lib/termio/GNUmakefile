#
#	Makefile for EPH lib ( Termio. )
#
#		by S.Yamane
#               modefyed by M.Nishiyama (Jan 22, 2011)
#

INC_DIR	= ../../include
LIB_DIR	= ../../lib

CC	= gcc -pipe

ifeq ($(DEBUG), 1)
CFLAGS	= -I$(INC_DIR) -g -DDEBUG
else
CFLAGS	= -I$(INC_DIR) -O
endif

ifeq ($(ENABLE_COLOR), 1)
CFLAGS	+= -DENABLE_COLOR
endif

ifeq ($(ENABLE_SYMBOL), 1)
CFLAGS	+= -DENABLE_SYMBOL
endif

ifeq ($(USE_FETCH), 1)
CFLAGS	+= -DUSE_FETCH
endif

ifeq ($(USE_CURL), 1)
CFLAGS	+= -DUSE_CURL
endif

OBJS	= eph_curses.o eph_more.o
SRCS	= eph_curses.c eph_more.c

LIBRARY	= $(LIB_DIR)/libeph.a

#
#
#
.c.o:
	$(CC) -c -o $@ $(CFLAGS) $<
	ar r $(LIBRARY) $*.o
	ranlib $(LIBRARY)

$(LIBRARY) : $(OBJS)

clean:
	@rm -f $(OBJS)

#
#	File Dependencies
#
$(OBJS) : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/ephcurs.h

#
#	End of Makefile
#
