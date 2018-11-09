bin_PROGRAMS = uc5App

uc5App_SOURCES = app-main.cpp 

uc5App_LDADD = $(AM_CPPFLAGS) ./uc5/libuc5.a \
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

SUBDIRS = uc5
