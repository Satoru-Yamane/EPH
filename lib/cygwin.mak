# *********************************************************
#
#       makefile for EPH lib ( cygwin gcc )
#
#       May. 10, 2011    M.Nishiyama
#       Sept.11, 2011
#
# *********************************************************

SUBDIR = astro misc termio

all:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) DEBUG=$(DEBUG) ENABLE_COLOR=$(ENABLE_COLOR) ENABLE_SYMBOL=$(ENABLE_SYMBOL) USE_FETCH=$(USE_FETCH) USE_CURL=$(USE_CURL) -f cygwin.mak; \
	cd ..; \
	done

clean:
	rm -f libeph.a
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) -f cygwin.mak clean; \
	cd ..; \
	done

#
#	end of Makefile
#
