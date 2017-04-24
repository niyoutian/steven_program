AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=-I$(top_srcdir)/platform/common/inc   \
	 -I$(top_srcdir)/app/test_log/inc 


test_log_LDADD =   \
                $(top_srcdir)/platform/common/src/libCommon.a

noinst_PROGRAMS = test_log
test_log_SOURCES=test_log.cpp
