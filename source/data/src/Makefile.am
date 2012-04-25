lib_LTLIBRARIES = libhpcl-data.la

libhpcl_data_la_SOURCES = \
                    host_info.cpp \
                    service_info.cpp \
                    data_server.cpp \
                    name_service_client.cpp \
                    data_component_factory.cpp

AM_CXXFLAGS = -Wall -Wextra -Weffc++ -std=c++0x

INCLUDES =	-I$(top_srcdir)/source/common/include/ \
			-I$(top_srcdir)/source/communication/include/ \
			-I$(top_srcdir)/source/data/include/

MAINTAINERCLEANFILES = Makefile.in
