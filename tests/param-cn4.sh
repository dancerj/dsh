#!/bin/sh
echo Check that cn4 option is working.
set -e
[ "`./dsh -recho -ma,b,c,d,e,f,g,h,i,j,k,l,m,n -Mcn4 | sort`" = "\
b /usr/local/bin/dsh -m b -m a -c -M -n4 -recho --
e /usr/local/bin/dsh -m e -m d -m c -c -M -n4 -recho --
h /usr/local/bin/dsh -m h -m g -m f -c -M -n4 -recho --
k /usr/local/bin/dsh -m k -m j -m i -c -M -n4 -recho --
n /usr/local/bin/dsh -m n -m m -m l -c -M -n4 -recho --" ]; 
