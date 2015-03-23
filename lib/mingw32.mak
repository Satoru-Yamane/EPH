# *********************************************************
#
#       makefile for EPH lib ( MinGW32 gcc )
#
#       May. 11, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

SUBDIR = astro misc termio

all:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) DEBUG=$(DEBUG) ENABLE_COLOR=$(ENABLE_COLOR) ENABLE_SYMBOL=$(ENABLE_SYMBOL) USE_CURL=$(USE_CURL) CURL_DIR=$(CURL_DIR) -f mingw32.mak; \
	cd ..; \
	done

clean:
	rm -f libeph.a
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) -f mingw32.mak clean; \
	cd ..; \
	done

#
#	end of Makefile
#
