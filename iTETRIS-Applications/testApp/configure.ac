dnl Process this file with autoconf to produce a configure script.
AC_INIT([testApp], [0.1])
AC_CONFIG_SRCDIR([src/app-main.cpp])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(foreign)

dnl the debug check has to be made before the program checks
dnl because it modifies CXXFLAGS
AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug],[enable sumo debugging code [default=no].])])
if test x$enable_debug = xyes; then
    AC_DEFINE(_DEBUG, 1, [Define to 1 in order to enable sumo debugging code.])
    CXXFLAGS="-g $CXXFLAGS"
    ac_enabled="$ac_enabled Debug"
else
    CXXFLAGS="-O2 -DNDEBUG $CXXFLAGS"
    ac_disabled="$ac_disabled Debug"
fi

dnl CXXFLAGS="-frtti $CXXFLAGS"

dnl Checks for programs
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AM_CPPFLAGS="-I$PWD/$srcdir/src -I$PWD/$srcdir/../baseApp/src -I$PWD/$srcdir/../baseApp/src/server -I$PWD/$srcdir/../baseApp/src/application -I$PWD/$srcdir/../baseApp/src/utils -I$PWD/$srcdir/../baseApp/src/foreign -I$PWD/$srcdir/../baseApp/src/application/helper -I$PWD/$srcdir/../baseApp/src/application/model -I$PWD/$srcdir/../../sumo/src -I$PWD/$srcdir/../../sumo/src/utils/common/ $AM_CPPFLAGS"
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_CXXFLAGS)

dnl ---------------------------------------------
dnl Checks proposed by autoscan.
dnl Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h malloc.h math.h stddef.h stdlib.h string.h sys/time.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([floor getcwd gettimeofday memset pow select socket sqrt])


AC_CONFIG_FILES([
src/test/Makefile
src/Makefile
Makefile])
AC_OUTPUT

AC_MSG_RESULT()
AC_MSG_RESULT([Optional features summary])
AC_MSG_RESULT([-------------------------])
AC_MSG_RESULT([Enabled:$ac_enabled])
AC_MSG_RESULT([Disabled:$ac_disabled])
