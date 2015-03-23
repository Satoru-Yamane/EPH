# *********************************************************
#
#       makefile for EPH ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.11, 2011
#       Feb. 22, 2012
#       June 18, 2012
#
# *********************************************************

!ifdef DEBUG
DEBUG_CFLAGS    = -DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CURL -DCURL_DIR=..\curl-7.26.0
!endif

SUB_DIR		=	lib src

MAKEFLAG	= $(DEBUG_CFLAGS) $(CURL_CFLAGS)

all:
	cd lib
	$(MAKE) $(MAKEFLAG) -f bccwin32.mak
	cd ..\src
	$(MAKE) $(MAKEFLAG) -f bccwin32.mak

clean:
	cd lib
	$(MAKE) -f bccwin32.mak clean
	cd ..\src
	$(MAKE) -f bccwin32.mak clean
