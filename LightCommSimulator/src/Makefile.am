bin_PROGRAMS = lightcomm

COMMON_LIBS = ./server/libserver.a \
liblightcomm.a \
./helper/libhelper.a \
./utils/exception/libexception.a \
./utils/xml/libxml.a \
./foreign/tcpip/libtcpip.a 

noinst_LIBRARIES = liblightcomm.a

liblightcomm_a_SOURCES = program-configuration.h program-configuration.cpp \
current-time.h current-time.cpp

lightcomm_SOURCES = lightcomm-main.cpp 

lightcomm_LDADD = $(AM_CPPFLAGS) $(COMMON_LIBS)

SUBDIRS = utils foreign server helper

EXTRA_DIST = config.h




