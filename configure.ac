# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright © 2010-2018 ANSSI. All Rights Reserved.
dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/install-gui.cpp)

dnl Checks for programs.
AC_PROG_CXX

[
if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -Wextra \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wcast-qual -Wcast-align -Wpointer-arith \
	-Wnested-externs"
fi
]

dnl Check for dependencies
if test -z "$PKG_CONFIG"; then
  PKG_CONFIG=pkg-config
fi
AC_SUBST(PKG_CONFIG)

AC_MSG_CHECKING(that pkg-config runs)
if "$PKG_CONFIG" --version > /dev/null 2>&1 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([*** $PKG_CONFIG missing ***

\'$PKG_CONFIG --version\' failed to run - make sure it is inside one of the
directories in your PATH environment variable!

pkg-config comes with the developement packages for GTK+-2.2, available at:

    http://www.gtk.org])
fi

AC_DEFUN([BUILD_DEPENDS], [
AC_MSG_CHECKING(that $1 (version >= $2) is installed)
if "$PKG_CONFIG" --atleast-version=$2 $1 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  if "$PKG_CONFIG" --exists $1 ; then
    ERRSTR=`"$PKG_CONFIG"  --modversion $1`
    AC_MSG_ERROR(Current version is only $ERRSTR)
  else
    AC_MSG_ERROR(Package is not installed)
  fi
fi
])

BUILD_DEPENDS(ClipWidgets, 1.0.0)

dnl Checks for header files.
AC_HEADER_STDC


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for QT lib
PKG_CHECK_MODULES(QT4_CORE, QtCore,,
    AC_MSG_ERROR([[
***
*** Qt4 Core is required.
***]])
)

PKG_CHECK_MODULES(QT4_GUI, QtGui,,
    AC_MSG_ERROR([[
***
*** Qt4 Gui is required.
***]])
)

AC_OUTPUT(Makefile src/Makefile scripts/Makefile)
