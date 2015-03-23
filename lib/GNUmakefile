# *********************************************************
#
#	makefile for EPH ( FreeBSD )
#
#	Jan 22, 2011	M.Nishiyama
#
# *********************************************************

SUBDIR = astro misc termio
# MAKE = gmake

all:
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) DEBUG=$(DEBUG) ENABLE_COLOR=$(ENABLE_COLOR) ENABLE_SYMBOL=$(ENABLE_SYMBOL) USE_FETCH=$(USE_FETCH) USE_CURL=$(USE_CURL); \
	cd ..; \
	done

clean:
	rm -f libeph.a
	for subdir in $(SUBDIR); do \
	cd $$subdir && $(MAKE) clean; \
	cd ..; \
	done

#
#	end of Makefile
#
