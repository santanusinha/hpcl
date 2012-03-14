lib_LTLIBRARIES = libhpcl-remote-exec.la

libhpcl_remote_exec_la_SOURCES = \
                child_process.cpp \
                exec_client.cpp \
                exec_server.cpp

AM_CXXFLAGS = -Wall -Wextra -Weffc++ -std=c++0x

INCLUDES =	-I$(top_srcdir)/source/common/include/ \
            -I$(top_srcdir)/source/communication/include/ \
            -I$(top_srcdir)/source/remote-exec/include/

MAINTAINERCLEANFILES = Makefile.in
