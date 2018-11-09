bin_PROGRAMS = uc1App

uc1App_SOURCES = app-main.cpp 

uc1App_LDADD = $(AM_CPPFLAGS) ./uc1/libuc1.a \
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

SUBDIRS = uc1
