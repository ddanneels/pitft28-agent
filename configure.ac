AC_INIT([pitft_agent], [0.1])
AM_INIT_AUTOMAKE([subdir-objects foreign])
AC_PROG_CC_C99

CFLAGS+=" -g -std=c11 -Wall -Wno-error"

PKG_CHECK_MODULES([LIBEVDEV], [libevdev >= 1.6.0],,
  AC_MSG_ERROR([libevdev 1.6.0 or newer not found.])
)

PKG_CHECK_MODULES([BLUEZ], [bluez >= 4.0.0],,
  AC_MSG_ERROR([BlueZ 4.0.0 or newer not found.])
)

# PKG_CHECK_MODULES([FREETYPE2], [freetype2],,
#   AC_MSG_ERROR([FreeType 2 not found.])
# )

# PKG_CHECK_MODULES([TSLIB], [tslib >= 1.10],,
#   AC_MSG_ERROR([libts 1.10 or newer not found.])
# )

AC_CHECK_LIB([wiringPi], [wiringPiSetup],,
  AC_MSG_ERROR([wiringPi not found.])
)

AC_CHECK_LIB([sensors], [sensors_get_value],,
  AC_MSG_ERROR([libsensors not found.])
)

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h dirent.h])

AC_CHECK_FUNCS([mmap ioctl strerror])


# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

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



