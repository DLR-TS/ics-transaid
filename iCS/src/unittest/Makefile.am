XERCES_LIBS = -l$(LIB_XERCES)

if WITH_GTEST
bin_PROGRAMS = ics-unittest
ics_unittest_SOURCES = iCSSumoFormat_unitTests.cpp

ics_unittest_LDFLAGS = -lgtest_main -lgtest -pthread

ics_unittest_LDADD   = \
../ics/configfile_parsers/sumoMapParser/SUMOdigital-map.o \
../utils/geom/libgeom.a \
../utils/xml/libxml.a \
../utils/common/libcommon.a \
../utils/options/liboptions.a \
../utils/common/StringUtils.o \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
$(XERCES_LIBS)

endif