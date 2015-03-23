# *********************************************************
#
#       makefile for EPH ( cygwin gcc )
#
#       May. 10, 2011    M.Nishiyama
#       Sept.12, 2011
#
# *********************************************************

INC_DIR	= ../include
LIB_DIR	= ../lib
BIN_DIR	= ../bin

CC	= gcc  -pipe

ifeq ($(DEBUG), 1)
DEBUG_CFLAGS	= -g -DDEBUG
else
DEBUG_CFLAGS	= -O
endif

CFLAGS	= -I$(INC_DIR) -I/usr/include/ncurses -DUSE_REGEX=1 -DKANJI=1 -DSJIS=1 $(DEBUG_CFLAGS)

LDFLAGS	+= -lncurses

ifeq ($(ENABLE_COLOR), 1)
CFLAGS	+= -DENABLE_COLOR
endif

ifeq ($(ENABLE_SYMBOL), 1)
CFLAGS	+= -DENABLE_SYMBOL
endif

ifeq ($(USE_CURL), 1)
CFLAGS	+= -DUSE_CURL
CURL_LIB = -L/usr/local/lib -lcurl
endif

ifeq ($(USE_FETCH), 1)
CFLAGS	+= -DUSE_FETCH
FETCH_LIB = -L/usr/local/lib -lfetch -lssl
endif

PROGRAM	= $(BIN_DIR)/eph

SRCS	= eph_calc.c eph_id.c input.c query.c eph_eph.c \
		eph_main.c output.c sys_ctrl.c eph_config.c

OBJS	= eph_calc.o eph_id.o input.o query.o eph_eph.o \
		eph_main.o output.o sys_ctrl.o eph_config.o

EPH_LIB	= $(LIB_DIR)/libeph.a

#
#	Final targets
#
all : $(PROGRAM)

$(PROGRAM) : $(OBJS) $(EPH_LIB)
	$(CC) -o $(PROGRAM) $(OBJS) $(EPH_LIB) $(LDFLAGS) -lm -lncurses $(CURL_LIB) $(FETCH_LIB)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	@rm -f $(OBJS) $(PROGRAM)

#
#	file dependencies
#
eph_calc.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h
eph_id.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
			$(INC_DIR)/filer.h
eph_utl.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h
input.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
			$(INC_DIR)/getval.h $(INC_DIR)/regex.h
query.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
			$(INC_DIR)/filer.h $(INC_DIR)/getval.h $(INC_DIR)/regex.h
eph_eph.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
			$(INC_DIR)/filer.h
eph_main.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
			$(INC_DIR)/getval.h $(INC_DIR)/ephcurs.h
output.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h
sys_ctrl.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h
eph_config.o : $(INC_DIR)/eph.h $(INC_DIR)/getval.h

#
#	end of Makefile
#
