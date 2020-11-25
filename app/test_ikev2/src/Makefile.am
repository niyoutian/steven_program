AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=-I$(top_srcdir)/platform/common/inc   \
	 -I$(top_srcdir)/platform/net/inc   \
	 -I$(top_srcdir)/app/test_net/inc 


ikeMgr_LDADD =   \
		$(top_srcdir)/platform/net/src/libNet.a	\
                $(top_srcdir)/platform/common/src/libCommon.a	

noinst_PROGRAMS = ikeMgr
ikeMgr_SOURCES=ikeMgr.cpp
