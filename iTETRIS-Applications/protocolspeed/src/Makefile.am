bin_PROGRAMS = protocolspeedApp

protocolspeedApp_SOURCES = app-main.cpp 

protocolspeedApp_LDADD = $(AM_CPPFLAGS) ./speed/libspeed.a \
../../baseApp/src/server/payload.o \
../../baseApp/src/application/traci-helper.o \
../../baseApp/src/server/libserver.a \
../../baseApp/src/application/libapplication.a \
../../baseApp/src/libbaseApp.a \
../../baseApp/src/application/model/libmodel.a \
../../baseApp/src/application/helper/libhelper.a \
../../baseApp/src/utils/exception/libexception.a \
../../baseApp/src/utils/log/liblog.a \
../../baseApp/src/utils/xml/libxml.a \
../../../sumo/src/foreign/tcpip/libtcpip.a 

SUBDIRS = speed
