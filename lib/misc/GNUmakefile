#
#	Makefile for EPH lib ( Misc. )
#
#		by S.Yamane
#		by K.Osada
#               modefyed by M.Nishiyama (Jan 22, 2011)
#

INC_DIR	= ../../include
LIB_DIR	= ../../lib

CC	= gcc  -pipe

ifeq ($(DEBUG), 1)
CFLAGS  = -I$(INC_DIR) -g -DDEBUG -DUSE_REGEX=1
else 
CFLAGS  = -I$(INC_DIR) -O -DUSE_REGEX=1
endif

ifeq ($(FAST_DEBUG), 1)
CFLAGS  += -DFAST_DEBUG
endif

ifeq ($(ENABLE_COLOR), 1)
CFLAGS  += -DENABLE_COLOR
endif

ifeq ($(ENABLE_SYMBOL), 1)
CFLAGS  += -DENABLE_SYMBOL
endif

ifeq ($(USE_FETCH), 1)
CFLAGS	+= -DUSE_FETCH
endif

ifeq ($(USE_CURL), 1)
CFLAGS  += -DUSE_CURL -I/usr/local/include
endif

OBJS	= lib_out.o out_calc.o get_fn.o filer.o menu_man.o mid.o regex.o \
		getval.o eph_select.o sel_dev.o time_sys.o fmt_gets.o s2v_v2s.o \
		file_sel.o imp_mpcelm.o

SRCS	= out_calc.c fmt_gets.c filer.c lib_out.c mid.c regex.c getval.c \
		eph_select.c sel_dev.c menu_man.c time_sys.c get_fn.c s2v_v2s.c \
		file_sel.c imp_mpcelm.c

LIBRARY	= $(LIB_DIR)/libeph.a

#
#
#
.c.o:
	$(CC) -c $(CFLAGS) $<
	ar r $(LIBRARY) $*.o
	ranlib $(LIBRARY)

$(LIBRARY) : $(OBJS)

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

#
#	End of Makefile
#
