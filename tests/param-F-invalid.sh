#! /bin/sh
# test to check that -F parameter fails properly.
./dsh -recho -M -m 1,2 -F 10 -c
case $? in
    0)
	;;
    *)
	exit 1;;
esac

./dsh -recho -M -m 1,2 -F 10 -w
case $? in
    1)
	;;
    *)
	exit 1;;
esac

