#!/bin/sh
# test to check that -m parameter checking is right.
set -ex
[ "`./dsh -M -recho -m a,b,c,c,d `" = "\
d: d
c: c
b: b
a: a" ]; 
