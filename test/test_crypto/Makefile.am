AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=	-I$(top_srcdir)/platform/common/inc   \
	    	-I$(top_srcdir)/algorithms/symmCrypto/inc   \
	 	-I$(top_srcdir)/test/test_crypto/inc 


test_crypto_LDADD =   \
		$(top_srcdir)/algorithms/symmCrypto/src/libSymmCrypto.a	\
                $(top_srcdir)/platform/common/src/libCommon.a


noinst_PROGRAMS = test_crypto
test_crypto_SOURCES=test_crypto.cpp
