#! /bin/sh
# test to check that -f parameter fails properly.
./dsh -recho -M -f ${srcdir}/tests/nonexistent.file 
case $? in
    1)
	exit 0;;
    *)
	exit 1;;
esac
