XERCES_LIBS = -l$(LIB_XERCES)
GEOGRAPHIC_LIBS = -l$(LIB_GEOGRAPHIC)

bin_PROGRAMS = iCS

COMMON_LIBS = ./ics/libics.a \
./ics/traffic_sim_communicator/libtrafficsimulatorcommunicator.a \
./ics/wirelesscom_sim_communicator/libwirelesscommunicationsimulatorcommunicator.a \
./ics/applications_manager/libapplicationsmanager.a \
./ics/wirelesscom_sim_message_tracker/libwirelesscommunicationmessagetracker.a \
./ics/facilities/libicsfacilities.a \
./ics/facilities/stationFacilities/libstationfacilities.a \
./ics/facilities/messageFacilities/libmessagefacilities.a \
./ics/facilities/mapFacilities/libmapfacilities.a \
./ics/facilities/mapFacilities/libmapfacilities.a \
./ics/facilities/mapFacilities/road/libmapfacilitiesroadelements.a \
./ics/configfile_parsers/libconfigfileparsers.a \
./ics/configfile_parsers/sumoMapParser/libconfigfileparserssumomap.a \
./utils/ics/log/libicslog.a \
./utils/ics/geometric/libutilsicsgeometric.a \
./utils/ics/libutilsics.a \
./utils/options/liboptions.a \
./utils/xml/libxml.a \
./utils/geom/libgeom.a \
./utils/common/libcommon.a \
./utils/iodevices/libiodevices.a \
./foreign/tcpip/libtcpip.a \
$(XERCES_LIBS) \
$(GEOGRAPHIC_LIBS) \
-lpthread


iCS_SOURCES = iCS_main.cpp

iCS_LDFLAGS = $(XERCES_LDFLAGS) $(GEOGRAPHIC_LDFLAGS)

iCS_LDADD   =  $(COMMON_LIBS) 



SUBDIRS = foreign utils ics unittest

EXTRA_DIST = config.h config.h.in \
windows_config.h

if WITH_VERSION_H
all: version.h
version.h: .svn/entries
	../tools/build/version.py
endif
