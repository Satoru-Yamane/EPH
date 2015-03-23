# *********************************************************
#
#       makefile for EPH ( Borland bcc )
#
#       May.  7, 2011    M.Nishiyama
#       Sept.11, 2011
#       Feb. 22, 2012
#
# *********************************************************

!ifdef DEBUG
MAKEFLAG	=	-DDEBUG
!endif

!ifdef USE_CURL
CURL_CFLAGS	= -DUSE_CURL -DCURL_DIR=..\curl-7.24.0
!endif

SUB_DIR		=	lib src

all:
	cd lib
	$(MAKE) $(MAKEFLAG) $(CURL_CFLAGS) -f bccwin32.mak
	cd ..\src
	$(MAKE) $(MAKEFLAG) $(CURL_CFLAGS) -f bccwin32.mak

clean:
	cd lib
	$(MAKE) -f bccwin32.mak clean
	cd ..\src
	$(MAKE) -f bccwin32.mak clean
