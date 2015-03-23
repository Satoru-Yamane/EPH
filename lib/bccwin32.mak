# *********************************************************
#
#       makefile for EPH ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.11, 2011
#
# *********************************************************

SUBDIR	= astro misc termio

!ifdef DEBUG
DEBUG_FLAGS	=	-DDEBUG
!endif

!ifdef USE_CURL
CURL_FLAGS	= -DUSE_CURL -DCURL_DIR=$(CURL_DIR)
!endif

MAKEFLAGS	= $(DEBUG_FLAGS) $(CURL_FLAGS)

all:
	cd astro
	$(MAKE) $(MAKEFLAGS) -f bccwin32.mak
	cd ..\misc
	$(MAKE) $(MAKEFLAGS) -f bccwin32.mak
	cd ..\termio
	$(MAKE) $(MAKEFLAGS) -f bccwin32.mak

clean:
	del eph.lib
	cd astro
	$(MAKE) -f bccwin32.mak clean
	cd ..\misc
	$(MAKE) -f bccwin32.mak clean
	cd ..\termio
	$(MAKE) -f bccwin32.mak clean
