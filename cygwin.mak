# *********************************************************
#
#       makefile for EPH ( cygwin gcc )
#
#       May. 10, 2011    M.Nishiyama
#       Sept.12, 2011
#
# *********************************************************

SUBDIR = lib src
DESTDIR = /usr/local/bin

# Enable debaug statement and make for gdb
# DEBUG	=	1

# Set DISABLE_COLOR to on, build with monochrome mode
# DISABLE_COLOR	=	1

# Set ENABLE_SYMBOL to on, using ruled line(virtical/holizontal and corner)
# ENABLE_SYMBOL	=	1

# Set USE_CURL to on, using libcurl
# USE_FETCH	=	1

# Set USE_CURL to on, using libcurl
# USE_CURL	=	1

ifeq ($(DISABLE_COLOR), 1)
ENABLE_COLOR	=	0
else
ENABLE_COLOR	=	1
endif

ifeq ($(USE_CURL), 1)
USE_FETCH	=	0
endif

all:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) DEBUG=$(DEBUG) ENABLE_COLOR=$(ENABLE_COLOR) ENABLE_SYMBOL=$(ENABLE_SYMBOL) USE_FETCH=$(USE_FETCH) USE_CURL=$(USE_CURL) CURL_DIR=$(CURL_DIR) -f cygwin.mak ; \
	cd ..; \
	done

install: bin/eph
	install -c -s $< $(DESTDIR)

clean:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) -f cygwin.mak clean; \
	cd ..; \
	done

bin/eph: all

#
#	end of Makefile
#
