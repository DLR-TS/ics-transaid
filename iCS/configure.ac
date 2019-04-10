dnl Process this file with autoconf to produce a configure script.
AC_INIT(iTETRIS, 0.1.0)
AC_CONFIG_SRCDIR(src/iCS_main.cpp)
AC_CANONICAL_SYSTEM

dnl create an config-header
AC_CONFIG_HEADER(src/config.h)

dnl enable automake-support
AM_INIT_AUTOMAKE([gnu dist-zip tar-ustar dist-bzip2])


dnl the debug check has to be made before the program checks
dnl because it modifies CXXFLAGS
AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug],[enable sumo debugging code [default=no].])])
if test x$enable_debug = xyes; then
    AC_DEFINE(_DEBUG, 1, [Define to 1 in order to enable sumo debugging code.])
    CXXFLAGS="-g $CXXFLAGS -std=c++11"
    ac_enabled="$ac_enabled Debug"
else
    CXXFLAGS="-O2 -DNDEBUG $CXXFLAGS -std=c++11"
    ac_disabled="$ac_disabled Debug"
fi

AC_DEFINE([SUMOReal], [double], [defines the precision of floats])
AC_DEFINE([POSITION_EPS], [0.1], [defines the epsilon to use on position comparison])
AC_DEFINE([NUMERICAL_EPS], [0.001], [defines the epsilon to use on general floating point comparison])
AC_DEFINE([OUTPUT_ACCURACY], [2], [defines the number of digits after the comma in output])
AC_DEFINE([GEO_OUTPUT_ACCURACY], [6], [defines the number of digits after the comma in geo-coordinates output])
AC_DEFINE([SUMOLong], [long long int], [defines a long]) 


dnl disable sumo related instructions
AC_ARG_ENABLE([sumo], [AS_HELP_STRING([--disable-sumo],[Do not execute SUMO instructions.])])
if test x$enable_sumo != xno; then
    AC_DEFINE([SUMO_ON], 1, [SUMO is active])
    ac_enabled="$ac_enabled SumoOn"
else
    ac_disabled="$ac_disabled SumoOn"
fi

dnl disable ns-3 related instructions
AC_ARG_ENABLE([ns3], [AS_HELP_STRING([--disable-ns3],[Do not execute ns-3 instructions.])])
if test x$enable_ns3 != xno; then
    AC_DEFINE([NS3_ON], 1, [ns-3 is active])
    ac_enabled="$ac_enabled ns-3On"
else    
    ac_disabled="$ac_disabled ns-3On"
fi

dnl disable Traffic Applications related instructions
AC_ARG_ENABLE([applications], [AS_HELP_STRING([--disable-applications],[Do not execute Traffic Applications instructions.])])
if test x$enable_applications != xno; then
    AC_DEFINE([APPLICATIONS_ON], 1, [Applications are active])
    ac_enabled="$ac_enabled ApplicationsOn"
else    
    ac_disabled="$ac_disabled ApplicationsOn"
fi

dnl disable Logging
AC_ARG_ENABLE([log], [AS_HELP_STRING([--disable-log],[Write iCS information to a log file.])])
if test x$enable_log != xno; then
    AC_DEFINE([LOG_ON], 1, [Logging is active])
    ac_enabled="$ac_enabled LogOn"
else    
    ac_disabled="$ac_disabled LogOn"
fi


dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl enable libtool-support
dnl gives a deprecated warning for RANLIB with newer libtools
AC_PROG_RANLIB


dnl Check for enable-features that shall go into config.h


dnl   ... for xerces 3.0
PKG_CHECK_MODULES([XERCES], [xerces-c >= 3.0],
                  [AC_DEFINE([XERCES3_SIZE_t],[XMLSize_t],[Define length for Xerces 3.])],
                  [AC_DEFINE([XERCES3_SIZE_t],[unsigned int],[Define length for Xerces 2.])])



AM_CONDITIONAL([WITH_VERSION_H], test -f .svn/entries)
if test -f .svn/entries; then
   AC_DEFINE([HAVE_VERSION_H], [1], [Define if auto-generated version.h should be used.])
else
   AC_DEFINE_UNQUOTED(VERSION_STRING, "$PACKAGE_VERSION", [Defined if auto-generated version.h is unavailable.])
fi

AM_CONDITIONAL([WITH_GUI], [false])
AM_CONDITIONAL([TRACI], [false])

dnl ---------------------------------------------
dnl Platform specific stuff
dnl - - - - - - - - - - - - - - - - - - - - - - -

case "$host" in
   *-cygwin*)
   AC_DEFINE([HAVE_CYGWIN], [1], [Define if compiling under cygwin])
   AC_DEFINE([WIN32], [1], [Xerces and Fox expect this to be defined (cygwin)])
   AC_DEFINE([NOMINMAX], [1], [Including windows.h gives errors otherwise (cygwin)])
   ;;
esac


dnl ---------------------------------------------
dnl check for libraries...
dnl - - - - - - - - - - - - - - - - - - - - - - -

dnl  ... for xerces
AC_ARG_WITH([xerces], [AS_HELP_STRING([--with-xerces=DIR],[where Xerces-C is installed (libraries in DIR/lib, headers in DIR/include).])])
if test x"$with_xerces" != x; then
   if test x"$with_xerces_libraries" = x; then
      with_xerces_libraries=$with_xerces/lib
   fi
   if test x"$with_xerces_includes" = x; then
      with_xerces_includes=$with_xerces/include
   fi
fi

dnl   ... for libs
LIB_XERCES="xerces-c"
AC_ARG_WITH([xerces-libraries], [AS_HELP_STRING([--with-xerces-libraries=DIR],[where the Xerces-C library is installed.])])
if test x"$with_xerces_libraries" = x; then
   AC_CHECK_LIB([$LIB_XERCES], [main], [true],
                [AC_MSG_ERROR([xerces-lib not found. Specify path with option --with-xerces-libraries=DIR.])])
else
   save_ldflags=$LDFLAGS
   LDFLAGS="-L$with_xerces_libraries"
   export LDFLAGS
   AC_CHECK_LIB([$LIB_XERCES], [main], [XERCES_LDFLAGS=$LDFLAGS],
                [AC_MSG_ERROR([xerces-lib not found in "$with_xerces_libraries".])])
   LDFLAGS=$save_ldflags
   AC_SUBST(XERCES_LDFLAGS)
fi
AC_SUBST(LIB_XERCES)


dnl   ... for includes
AC_ARG_WITH([xerces-includes], [AS_HELP_STRING([--with-xerces-includes=DIR],[where the Xerces-C includes are.])])
save_cppflags=$CPPFLAGS
if test x"$with_xerces_includes" = x; then
    CPPFLAGS="$CPPFLAGS -I/usr/local/include -I/usr/include"
    export CPPFLAGS
    AC_CHECK_HEADER([xercesc/util/XercesVersion.hpp], [AM_CPPFLAGS="$CPPFLAGS $AM_CPPFLAGS"],
                    [AC_MSG_ERROR([xerces-includes not found. Specify path with option --with-xerces-includes=DIR.])])
else
    CPPFLAGS="-I$with_xerces_includes"
    export CPPFLAGS
    AC_CHECK_HEADER([xercesc/util/XercesVersion.hpp], [AM_CPPFLAGS="$CPPFLAGS $AM_CPPFLAGS"],
                    [AC_MSG_ERROR([xerces-includes not found in "$with_xerces_includes".])])
fi
CPPFLAGS=$save_cppflags


dnl  ... for Geographic library (for latitude, longitude and altitude conversion)
dnl  ... for Geographic
AC_ARG_WITH([geographic], [AS_HELP_STRING([--with-geographic=DIR],[where Geographic is installed (libraries in DIR/lib, headers in DIR/include).])])
if test x"$with_geographic" != x; then
   if test x"$with_geographic_libraries" = x; then
      if test -e $with_geographic/lib64; then
          with_geographic_libraries=$with_geographic/lib64
      else
          with_geographic_libraries=$with_geographic/lib
      fi
   fi
   if test x"$with_geographic_includes" = x; then
      with_geographic_includes=$with_geographic/include
   fi
fi

dnl   ... for libs
LIB_GEOGRAPHIC="Geographic"
AC_ARG_WITH([geographic-libraries], [AS_HELP_STRING([--with-geographic-libraries=DIR],[where the Geographic library is installed.])])
if test x"$with_geographic_libraries" = x; then
   AC_CHECK_LIB([$LIB_GEOGRAPHIC], [main], [true],
                [AC_MSG_ERROR([Geographic-lib not found. Specify path with option --with-geographic-libraries=DIR.])])
else
   save_ldflags=$LDFLAGS
   LDFLAGS="-L$with_geographic_libraries"
   export LDFLAGS
   AC_CHECK_LIB([$LIB_GEOGRAPHIC], [main], [GEOGRAPHIC_LDFLAGS=$LDFLAGS],
                [AC_MSG_ERROR([Geographic-lib not found in "$with_geographic_libraries".])])
   LDFLAGS=$save_ldflags
   AC_SUBST(GEOGRAPHIC_LDFLAGS)
fi
AC_SUBST(LIB_GEOGRAPHIC)
dnl -lGeographic

dnl   ... for includes
AC_ARG_WITH([geographic-includes], [AS_HELP_STRING([--with-geographic-includes=DIR],[where the Geographic includes are.])])
save_cppflags=$CPPFLAGS
if test x"$with_geographic_includes" = x; then
    CPPFLAGS="$CPPFLAGS -I/usr/local/include"
    export CPPFLAGS
else
    CPPFLAGS="-I$with_geographic_includes $save_cppflags"
    export CPPFLAGS
fi
dnl CPPFLAGS=$save_cppflags





dnl - - - - - - - - - - - - - - - - - - - - - - -
dnl  ... for gtest
AC_ARG_WITH([gtest], [AS_HELP_STRING([--with-gtest=DIR],[where googletest is installed (libraries in DIR/lib, headers in DIR/include).])])
if test x"$with_gtest" == x; then
    with_gtest=no
fi
if test x"$with_gtest" != xno; then
    LIB_GTEST="gtest"
    save_ldflags=$LDFLAGS
    LDFLAGS="-L$with_gtest/lib"
    export LDFLAGS
    AC_CHECK_LIB([$LIB_GTEST], [main], [GTEST_LDFLAGS=$LDFLAGS],
                 [AC_MSG_ERROR([gtest-lib not found in "$with_gtest/lib".])])
    LDFLAGS=$save_ldflags
    AC_SUBST(GTEST_LDFLAGS)
    AC_SUBST(LIB_GTEST)
    AM_CPPFLAGS="-I$with_gtest/include $AM_CPPFLAGS"
#    save_cppflags=$CPPFLAGS
#    CPPFLAGS="-I$with_gtest/include"
#    export CPPFLAGS
#    AC_CHECK_HEADER([gtest/gtest.h], [AM_CPPFLAGS="$CPPFLAGS $AM_CPPFLAGS"],
#                    [AC_MSG_ERROR([gtest-includes not found in "$with_gtest/include".])])
#    CPPFLAGS=$save_cppflags
fi
AM_CONDITIONAL([WITH_GTEST], [test x"$with_gtest" != xno])
if test x"$with_gtest" != xno; then
    ac_enabled="$ac_enabled UnitTests"
else
    ac_disabled="$ac_disabled UnitTests"
fi

SUMOUTILS_LDFLAGS=-L$PWD/$srcdir/../lib
AC_SUBST(SUMOUTILS_LDFLAGS)

AM_CPPFLAGS="-I$PWD/$srcdir/src -I$PWD/$srcdir/../sumo/src $AM_CPPFLAGS"
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_CXXFLAGS)


dnl ---------------------------------------------
dnl Checks proposed by autoscan.
dnl Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h malloc.h math.h stddef.h stdlib.h string.h sys/time.h unistd.h wchar.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([floor getcwd gettimeofday memset pow select socket sqrt])


dnl ---------------------------------------------
dnl add here all your Makefiles. These will be created by configure
AC_CONFIG_FILES([
bin/Makefile
src/Makefile
src/unittest/Makefile
src/ics/Makefile
src/ics/traffic_sim_communicator/Makefile
src/ics/wirelesscom_sim_communicator/Makefile
src/ics/wirelesscom_sim_message_tracker/Makefile
src/ics/applications_manager/Makefile
src/ics/configfile_parsers/Makefile
src/ics/configfile_parsers/sumoMapParser/Makefile
src/ics/facilities/Makefile
src/ics/facilities/mapFacilities/Makefile
src/ics/facilities/mapFacilities/road/Makefile
src/ics/facilities/stationFacilities/Makefile
src/ics/facilities/messageFacilities/Makefile
src/utils/Makefile
src/utils/ics/geometric/Makefile
src/utils/ics/log/Makefile
src/utils/ics/Makefile
Makefile])
AC_OUTPUT

dnl ---------------------------------------------
dnl Summary

AC_MSG_RESULT()
AC_MSG_RESULT([Optional features summary])
AC_MSG_RESULT([-------------------------])
AC_MSG_RESULT([Enabled:$ac_enabled])
AC_MSG_RESULT([Disabled:$ac_disabled])
