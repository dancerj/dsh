dnl Process this file with autoconf to produce a configure script.

AC_INIT(dsh, 0.22.0)
AM_INIT_AUTOMAKE(dsh, 0.22.0)
AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_MAKE_SET
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

dnl Check if it is a GNU getopt that munges parameter option orderings.
dnl and stops munging if + is specified.

AH_TEMPLATE([GETOPT_WITH_GNU_REORDERING_EXTENTION], 
	    [If getopt has an extention of reordering the parameters before processing, and it can be cancelled with adding a "+" at the start like GNU getopt, enable this option.])


getopt_gnu=0
AC_MSG_CHECKING([getopt with a GNU opton reordering extention])
AC_TRY_RUN([
#define _GNU_SOURCE
#include <getopt.h>
int main()
{
  char *opts@<:@@:>@ = {"hoge", "-a", "hostname", "-c", 0};
  int c;

  while (-1 != (c = getopt (4, opts, "ac")))
    {
      switch (c)
	{
	case 'c':
	  exit (0);
	}
    }
  exit (1);
}
], AC_TRY_RUN([
#define _GNU_SOURCE
#include <getopt.h>
int main()
{
  char *opts@<:@@:>@ = {"hoge", "-a", "hostname", "-c", 0};
  int c;

  while (-1 != (c = getopt (4, opts, "+ac")))
    {
      switch (c)
	{
	case 'c':
	  exit (1);
	}
    }
  exit (0);
}], [getopt_gnu=yes], 
   [getopt_gnu=no_workaround]
   ),
   [getopt_gnu=posix],
   [getopt_gnu=cross]
)

case ${getopt_gnu} in
  yes) AC_DEFINE([GETOPT_WITH_GNU_REORDERING_EXTENTION])
  AC_MSG_RESULT([yes, disabling]);;
  no_workaround) AC_MSG_RESULT([no, without workaround]) ;;
  posix) AC_MSG_RESULT([no, POSIX compliant]) ;;
  cross) AC_MSG_RESULT([cross-compiling, ignore]) ;;
  *)AC_MSG_RESULT([unexpected result :${getopt_gnu}])
esac

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_CONFIG_FILES([ po/Makefile.in m4/Makefile 
Makefile
])
AC_OUTPUT
