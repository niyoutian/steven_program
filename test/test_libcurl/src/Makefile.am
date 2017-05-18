AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=	-I$(top_srcdir)/platform/common/inc   \
	    	-I$(top_srcdir)/platform/net/inc   \
	 	-I$(top_srcdir)/test/test_libcurl/inc 


test_curl_LDADD =   \
		$(top_srcdir)/platform/net/src/libNet.a	\
                $(top_srcdir)/platform/common/src/libCommon.a

test_curl_LDADD += -lcurl

noinst_PROGRAMS = test_curl
test_curl_SOURCES=test_curl.cpp
