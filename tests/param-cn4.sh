#! /bin/sh
echo Check that cn4 option is working. 
set -e
[ "`./dsh -recho -ma,b,c,d,e,f,g,h,i,j,k,l,m,n -Mcn4 | sort`" = "\
a ${bindir}/dsh -m a -m b -m c -c -M -n4 -recho --
d ${bindir}/dsh -m d -m e -m f -c -M -n4 -recho --
g ${bindir}/dsh -m g -m h -m i -c -M -n4 -recho --
j ${bindir}/dsh -m j -m k -m l -c -M -n4 -recho --
m ${bindir}/dsh -m m -m n -c -M -n4 -recho --" ]; 
