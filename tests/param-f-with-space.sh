#! /bin/sh
# test to check that -f parameter is working with data with space.

set -e
[ "`./dsh -recho -M -f ${srcdir}/tests/list.space.file -f ${srcdir}/tests/list.space.file `" = \
"a: a
b: b
cc: cc
d: d" ]; 
