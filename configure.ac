AC_INIT([pitft_agent], [0.1])
AM_INIT_AUTOMAKE([subdir-objects foreign])
AC_PROG_CXX
AC_SUBST([AM_CXXFLAGS],["-std=c++17 -g -O2 -Wno-unused-function -Wno-cpp -Wall -Wno-error"])
PKG_CHECK_MODULES([LIBEVDEV], [libevdev >= 1.6.0])


# # Doxygen
# AC_PATH_PROG(DOXYGEN, [doxygen])
# if test "x$DOXYGEN" = "x"; then
# 	AC_MSG_WARN([doxygen not found - required for documentation])
# 	have_doxygen="no"
# else
# 	have_doxygen="yes"
# fi
# AM_CONDITIONAL([HAVE_DOXYGEN], [test "x$have_doxygen" = "xyes"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT



