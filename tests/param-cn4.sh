#!/bin/sh
echo Check that cn4 option is working. not
set -ex
[ "`./dsh -recho -ma,b,c,d,e,f,g,h,i,j,k,l,m,n -Mcn4 | sort`" = "\
b ${bindir}/dsh -m b -m a -c -M -n4 -recho --
e ${bindir}/dsh -m e -m d -m c -c -M -n4 -recho --
h ${bindir}/dsh -m h -m g -m f -c -M -n4 -recho --
k ${bindir}/dsh -m k -m j -m i -c -M -n4 -recho --
n ${bindir}/dsh -m n -m m -m l -c -M -n4 -recho --" ]; 
