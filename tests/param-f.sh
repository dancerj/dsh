#! /bin/sh
# test to check that -f parameter checking is right.
set -e
[ "`./dsh -recho -M -f ${srcdir}/tests/list.file -f ${srcdir}/tests/list.file `" = \
"a: a
b: b
cc: cc
d: d" ]; 
