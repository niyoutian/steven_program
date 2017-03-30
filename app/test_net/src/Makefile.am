AUTOMAKE_OPTIONS=foreign


INCLUDES=-I$(top_srcdir)/platform/common/inc   \
	 -I$(top_srcdir)/platform/net/inc   \
	 -I$(top_srcdir)/app/test_net/inc 


test_net_LDADD =   \
		$(top_srcdir)/platform/net/src/libNet.a	\
                $(top_srcdir)/platform/common/src/libCommon.a	

noinst_PROGRAMS = test_net
test_net_SOURCES=test_net.cpp
