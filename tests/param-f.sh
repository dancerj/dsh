#!/bin/sh
# test to check that -m parameter checking is right.
set -ex
[ "`./dsh -recho -M -f ${srcdir}/tests/list.file -f ${srcdir}/tests/list.file `" = \
"d: d
cc: cc
b: b
a: a" ]; 
