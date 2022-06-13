AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=	-I$(top_srcdir)/platform/common/inc   \
	    	-I$(top_srcdir)/algorithms/symmCrypto/inc   \
	 	-I$(top_srcdir)/test/test_crypto/inc 


test_crypto_LDADD =   \
		$(top_srcdir)/algorithms/symmCrypto/src/libSymmCrypto.a	\
                $(top_srcdir)/platform/common/src/libCommon.a \
		-L/home/steven/study2/openssl-1.1.1g -lssl -lcrypto -ldl


noinst_PROGRAMS = test_crypto
test_crypto_SOURCES=test_crypto.cpp

#如果需要对工程中某一个文件或库或者可执行文件设置，则在Makefile.am中加入"-g -O0"。注意：如果不写-O0，automake默认会加上"-O2"
#test_crypto_CXXFLAGS = -g -O0        整个app
#test_crypto_o_CXXFLAGS = -g -O0      单个文件
