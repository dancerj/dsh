#! /bin/sh
# test to check that -f parameter is working with data with space.

set -e
[ "`./dsh -recho -M -f ${srcdir}/tests/list.space.file -f ${srcdir}/tests/list.space.file `" = \
"d: d
cc: cc
b: b
a: a" ]; 
