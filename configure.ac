dnl Process this file with autoconf to produce a configure script.

AC_INIT(dsh, 0.20.3)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_LIBTOOL
AC_C_CONST

ALL_LINGUAS=ja
AM_GNU_GETTEXT([external])
AC_CHECK_FUNC(gettext,,AC_CHECK_LIB(intl,gettext,,AC_ERROR([Gettext not found])))
localedir='${datadir}/locale'
AC_SUBST(localedir)

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([asprintf strdup getopt_long getline])
AC_CHECK_LIB([dshconfig], [open_dshconfig],[],[AC_ERROR([dshconfig not found!!])])

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_OUTPUT([ po/Makefile.in m4/Makefile 
Makefile
])
