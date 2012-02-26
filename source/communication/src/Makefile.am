lib_LTLIBRARIES = libhpcl-communication.la

libhpcl_communication_la_SOURCES = \
                            meminfo.cpp \
                            socket.cpp \
                            socket_factory.cpp \
                            server_socket.cpp \
                            local_communicator.cpp

AM_CXXFLAGS = -Wall -Wextra -Weffc++ -std=c++0x -pthread

INCLUDES =	-I$(top_srcdir)/source/common/include/ \
            -I$(top_srcdir)/source/communication/include/

MAINTAINERCLEANFILES = Makefile.in
