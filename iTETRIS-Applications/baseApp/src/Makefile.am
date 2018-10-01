noinst_LIBRARIES = libbaseApp.a

libbaseApp_a_SOURCES = app-commands-subscriptions-constants.h \
structs.h \
program-configuration.h program-configuration.cpp \
current-time.h current-time.cpp

libbaseApp_a_LIBADD = ./server/libserver.a \
./application/libapplication.a \
./application/model/libmodel.a \
./application/model/test/libmodel_test.a \
./application/helper/libhelper.a \
./utils/exception/libexception.a \
./utils/log/liblog.a \
./utils/xml/libxml.a \
./foreign/tcpip/libtcpip.a 

SUBDIRS = utils foreign server application

EXTRA_DIST = config.h
