#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(steven_program, 1.0, niyoutian_0510@163.com)
AC_CONFIG_SRCDIR([platform/common/inc/mxLog.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
${CXXFLAGS="-g -O0"} #如果要对整个工程修改
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netinet/in.h sys/socket.h sys/time.h syslog.h stddef.h])
AM_INIT_AUTOMAKE

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gettimeofday memset regcomp select socket strerror strstr])

AC_CONFIG_FILES([Makefile
		 platform/Makefile
                 platform/common/Makefile
                 platform/common/src/Makefile
                 platform/net/Makefile
                 platform/net/src/Makefile
		 platform/mysql/Makefile
		 platform/mysql/src/Makefile
                 algorithms/Makefile
                 algorithms/avlTree/Makefile
                 algorithms/avlTree/src/Makefile
                 algorithms/hash/Makefile
                 algorithms/hash/src/Makefile
                 algorithms/symmCrypto/Makefile
                 algorithms/symmCrypto/src/Makefile
		 app/Makefile
		 app/test_log/Makefile
		 app/test_log/src/Makefile
                 app/test_net/Makefile
                 app/test_net/src/Makefile
                 app/test_ikev2/Makefile
                 app/test_ikev2/src/Makefile
                 app/test_avlTree/Makefile
                 app/test_avlTree/src/Makefile
		 app/test_mysql/Makefile
		 app/test_mysql/src/Makefile
		 test/Makefile
		 test/test_hash/Makefile
                 test/test_crypto/Makefile
		 test/test_libcurl/Makefile
		 test/test_libcurl/src/Makefile])
AC_OUTPUT
