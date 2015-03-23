# *********************************************************
#
#       makefile for EPH ( VisualSudio )
#         ex.) nmake USE_CURL=1 /F vcwin32.mak
#
#       Mar.  6, 2011    M.Nishiyama
#       Sept.10, 2011
#       Feb. 22, 2012
#       June 18, 2012
#
# *********************************************************

SUB_DIR	= lib src

# DEBUG = 1

!ifdef DEBUG
DEBUG_FLAG	= DEBUG=1
!endif

!ifdef USE_CURL
CURL_FLAG	= USE_CURL=1 CURL_DIR=..\curl-7.26.0
!endif

all:
	for %%A in ($(SUB_DIR)) do (cd %%A && $(MAKE) $(DEBUG_FLAG) $(CURL_FLAG) /F vcwin32.mak && cd ..)

clean:
	for %%A in ($(SUB_DIR)) do (cd %%A && $(MAKE) /F vcwin32.mak clean && cd ..)
