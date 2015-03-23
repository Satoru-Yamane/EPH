# *********************************************************
#
#       makefile for EPH lib - misc ( MinGW32 gcc )
#
#       May  11, 2011    M.Nishiyama
#       Sept.10, 2011
#       June 18, 2012
#
# *********************************************************

INC_DIR	= ../../include
LIB_DIR	= ../../lib
CURS_INC = ../../pdcurses
CURS_LIB = ../../pdcurses/win32

CC	= gcc  -pipe

ifeq ($(DEBUG), 1)
CFLAGS  = -I$(INC_DIR) -I$(CURS_INC) -g -DDEBUG -DUSE_REGEX=1
else 
CFLAGS  = -I$(INC_DIR) -I$(CURS_INC) -O -DUSE_REGEX=1
endif

ifeq ($(ENABLE_COLOR), 1)
CFLAGS  += -DENABLE_COLOR
endif

ifeq ($(ENABLE_SYMBOL), 1)
CFLAGS  += -DENABLE_SYMBOL
endif

ifeq ($(USE_CURL), 1)
CFLAGS	+= -I../../$(CURL_DIR)/include -DUSE_CURL -DCURL_STATICLIB
endif

OBJS	= lib_out.o out_calc.o get_fn.o filer.o menu_man.o mid.o regex.o \
		getval.o eph_select.o sel_dev.o time_sys.o fmt_gets.o s2v_v2s.o \
		file_sel.o imp_mpcelm.o edit_mpcurl.o edit_ttutc.o

SRCS	= out_calc.c fmt_gets.c filer.c lib_out.c mid.c regex.c getval.c \
		eph_select.c sel_dev.c menu_man.c time_sys.c get_fn.c s2v_v2s.c \
		file_sel.c imp_mpcelm.c edit_mpcurl.c edit_ttutc.c

EPH_LIB	= $(LIB_DIR)/libeph.a

#
#
#
.c.o:
	$(CC) -c $(CFLAGS) $<
	ar r $(EPH_LIB) $*.o
	ranlib $(EPH_LIB)

$(EPH_LIB) : $(OBJS)

clean:
		@rm -f $(OBJS)

#
#	File Dependencies
#
fmt_gets.o : $(INC_DIR)/menu.h $(INC_DIR)/ephcurs.h
filer.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
				$(INC_DIR)/filer.h
lib_out.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h \
				$(INC_DIR)/filer.h
regex.o : $(INC_DIR)/regex.h
getval.o : $(INC_DIR)/astrolib.h $(INC_DIR)/getval.h $(INC_DIR)/eph.h \
				$(INC_DIR)/menu.h $(INC_DIR)/regex.h
eph_select.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h $(INC_DIR)/ephcurs.h
sel_dev.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h
menu_man.o : $(INC_DIR)/menu.h $(INC_DIR)/ephcurs.h
time_sys.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h
get_fn.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h $(INC_DIR)/menu.h $(INC_DIR)/ephcurs.h
s2v_v2s.o : $(INC_DIR)/astrolib.h $(INC_DIR)/eph.h
file_sel.o : $(INC_DIR)/menu.h $(INC_DIR)/eph.h
imp_mpcelm.o : $(INC_DIR)/menu.h $(INC_DIR)/eph.h
edit_mpcurl.o: $(INC_DIR)/eph.h $(INC_DIR)/menu.h
edit_ttutc.o: $(INC_DIR)/eph.h $(INC_DIR)/menu.h

#
#	End of Makefile
#
