#! /bin/sh
# test to check that -rinvalidsomething errors out
./dsh -w -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	exit 0;;
    *) 
	exit 1;;
esac

./dsh -c -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	exit 0;;
    *) 
	exit 1;;
esac


