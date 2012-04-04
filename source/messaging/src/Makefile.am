lib_LTLIBRARIES = libhpcl-messaging.la

libhpcl_messaging_la_SOURCES = \
					message_client.cpp \
					message_component_factory.cpp \
					message_server.cpp
                
AM_CXXFLAGS = -Wall -Wextra -Weffc++ -std=c++0x -pthread

INCLUDES =	-I$(top_srcdir)/source/common/include/ \
            -I$(top_srcdir)/source/communication/include/ \
            -I$(top_srcdir)/source/messaging/include/

MAINTAINERCLEANFILES = Makefile.in
