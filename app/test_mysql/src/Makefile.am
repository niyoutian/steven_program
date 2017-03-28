AUTOMAKE_OPTIONS=foreign


INCLUDES=-I$(top_srcdir)/platform/common/inc   \
	 -I$(top_srcdir)/app/test_mysql/inc 


test_mysql_LDADD =   \
                $(top_srcdir)/platform/common/src/libCommon.a

noinst_PROGRAMS = test_mysql
test_mysql_SOURCES=test_mysql.cpp
