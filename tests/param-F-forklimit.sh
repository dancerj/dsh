#! /bin/sh
# test to check that -F parameter is working properly
date 
./dsh -v -rsleep -M -m 5,4,3,2,1 -F 3 
case $? in
    0)
	;;
    *)
	exit 1;;
esac
date
