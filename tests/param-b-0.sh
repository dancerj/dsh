#! /bin/sh
echo test to check that -b0 is rejected, and -b1 is accepted

./dsh -recho -b 0 -ma,b; 
case $? in
    1);;
    *) exit 1;;
esac


./dsh -recho -b -1 -ma,b; 
case $? in
    1);;
    *) exit 1;;
esac


./dsh -recho -b 1 -ma,b; 
case $? in
    0);;
    *) exit 1;;
esac

exit 0
