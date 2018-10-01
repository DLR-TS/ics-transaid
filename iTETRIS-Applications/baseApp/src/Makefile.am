bin_PROGRAMS = testApp

COMMON_LIBS = ./server/payload.o \
./application/traci-helper.o \
./server/libserver.a \
./application/libapplication.a \
libtestApp.a \
./application/model/libmodel.a \
./application/model/test/libmodel_test.a \
./application/helper/libhelper.a \
./utils/exception/libexception.a \
./utils/log/liblog.a \
./utils/xml/libxml.a \
./foreign/tcpip/libtcpip.a 

noinst_LIBRARIES = libtestApp.a

libtestApp_a_SOURCES = app-commands-subscriptions-constants.h \
structs.h \
program-configuration.h program-configuration.cpp \
current-time.h current-time.cpp

testApp_SOURCES = app-main.cpp 

testApp_LDADD = $(AM_CPPFLAGS) $(COMMON_LIBS)

SUBDIRS = utils foreign server application

EXTRA_DIST = config.h
