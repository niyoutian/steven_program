AM_CPPFLAGS=  
CXXFLAGS = 

noinst_LIBRARIES = libHash.a


libHash_a_SOURCES = \
        hashMd5.cpp \
	hashSha1.cpp \
	hashSha256.cpp

libHash_a_CXXFLAGS = -g -O0 \
                     -I$(top_srcdir)/platform/common/inc \
                     -I$(top_srcdir)/algorithms/hash/inc
