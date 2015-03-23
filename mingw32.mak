# *********************************************************
#
#       makefile for EPH ( MinGW32 gcc )
#         ex.) make USE_CURL=1 -f mingw32.mak
#
#       May. 11, 2011    M.Nishiyama
#       Sept.10, 2011
#       Feb. 22, 2012
#
# *********************************************************

SUBDIR = lib src

# Enable debaug statement and make for gdb
# DEBUG	=	1

# Set DISABLE_COLOR to on, build with monochrome mode
# DISABLE_COLOR	=	1

# Set ENABLE_SYMBOL to on, using ruled line(virtical/holizontal and corner)
# ENABLE_SYMBOL	=	1

# Set USE_CURL to on, using libcurl when downlod MPC orbial elements.
# USE_CURL	=	1

ifeq ($(USE_CURL), 1)
CURL_DIR	= ../curl-7.24.0
endif

ifeq ($(DISABLE_COLOR), 1)
ENABLE_COLOR	=	0
else
ENABLE_COLOR	=	1
endif

all:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) DEBUG=$(DEBUG) ENABLE_COLOR=$(ENABLE_COLOR) ENABLE_SYMBOL=$(ENABLE_SYMBOL) USE_CURL=$(USE_CURL) CURL_DIR=$(CURL_DIR) -f mingw32.mak ; \
	cd ..; \
	done

install: bin/eph
	install -c -s $< $(DESTDIR)

clean:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) -f mingw32.mak clean; \
	cd ..; \
	done

bin/eph: all

#
#	end of Makefile
#
