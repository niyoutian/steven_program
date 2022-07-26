AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=	-I$(top_srcdir)/platform/common/inc   \
	    	-I$(top_srcdir)/algorithms/hash/inc   \
	 	-I$(top_srcdir)/test/test_hash/inc 


test_hash_LDADD =   \
		$(top_srcdir)/algorithms/hash/src/libHash.a	\
                $(top_srcdir)/platform/common/src/libCommon.a


noinst_PROGRAMS = test_hash test_hmac
test_hash_SOURCES=test_hash.cpp

test_hmac_LDADD =   \
                $(top_srcdir)/algorithms/hash/src/libHash.a     \
                $(top_srcdir)/platform/common/src/libCommon.a

test_hmac_SOURCES=test_hmac.cpp
