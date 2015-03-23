#
#	Makefile for EPH lib ( astro. )
#
#		by S.Yamane
#		by K.Osada
#               modefyed by M.Nishiyama (Jan 22, 2011)
#

INC_DIR	= ../../include
LIB_DIR	= ../../lib

CC	= gcc  -pipe

ifeq ($(DEBUG), 1)
CFLAGS	= -I$(INC_DIR) -g -DDEBUG
else
CFLAGS	= -I$(INC_DIR) -O
endif

ifeq ($(DEBUG_ALONE), 1)
CFLAGS	+= -DDEBUG_ALONE
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
