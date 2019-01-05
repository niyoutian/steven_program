
AUTOMAKE_OPTIONS=foreign
SUBDIRS = avlTree hash
AM_CPPFLAGS=  \
-I$(top_srcdir)/platform/common/inc \
-I$(top_srcdir)/algorithms/avlTree/inc \
-I$(top_srcdir)/algorithms/hash/inc

noinst_LIBRARIES = libAlgorithms.a


libAlgorithms_a_SOURCES = \
        avlTree/src/AvlTreeNode.cpp \
	hash/src/hashMd5.cpp

