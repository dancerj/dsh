#! /bin/sh
# test to check that -m parameter checking is right.
set -e
[ "`./dsh -M -recho -m a,b,c,c,d `" = "\
a: a
b: b
c: c
d: d" ]; 
