lib_LTLIBRARIES = libhpcl-logging.la

libhpcl_logging_la_SOURCES = \
                remote_log_server.cpp \
                remote_logger.cpp \
                logging_component_factory.cpp

AM_CXXFLAGS = -Wall -Wextra -Weffc++ -std=c++0x -pthread

INCLUDES =	-I$(top_srcdir)/source/common/include/ \
            -I$(top_srcdir)/source/communication/include/ \
            -I$(top_srcdir)/source/logging/include/

MAINTAINERCLEANFILES = Makefile.in
