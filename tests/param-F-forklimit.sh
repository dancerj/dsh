#! /bin/sh
# test to check that -F parameter is working properly
date 
./dsh -v -rsleep -M -m 1,2,3,4,5 -F 3 
case $? in
    0)
	;;
    *)
	exit 1;;
esac
date
