AUTOMAKE_OPTIONS=foreign


AM_CPPFLAGS=-I$(top_srcdir)/platform/common/inc   	\
	 -I$(top_srcdir)/platform/mysql/inc	\
	 -I$(top_srcdir)/app/test_mysql/inc 


test_mysql_LDADD =   \
                $(top_srcdir)/platform/common/src/libCommon.a	\
		$(top_srcdir)/platform/mysql/src/libMysql.a	\
		-lmysqlclient

noinst_PROGRAMS = test_mysql
test_mysql_SOURCES=test_mysql.cpp
