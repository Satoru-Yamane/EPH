# *********************************************************
#
#       makefile for EPH lib ( VisualSudio )
#
#       Mar.  6, 2011    M.Nishiyama
#       Sept.10, 2011
#
# *********************************************************

# DEBUG = 1

!ifdef DEBUG
DEBUG_FLAGS    = DEBUG=1
!endif

!ifdef USE_CURL
CURL_FLAGS     = USE_CURL=1 CURL_DIR=$(CURL_DIR)
!endif



SUBDIR	= astro misc termio

all:
	for %%A in ($(SUBDIR)) do (cd %%A && $(MAKE) $(CURL_FLAGS) $(DEBUG_FLAGS) /F vcwin32.mak && cd ..)

clean:
	del eph.lib
	for %%A in ($(SUBDIR)) do (cd %%A && $(MAKE) -f vcwin32.mak clean && cd ..)
