#! /bin/sh
# test to check that -rinvalidsomething errors out

./dsh -w -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac

./dsh -c -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac

./dsh -w -r true -m a,b,c,d
case $? in
    0)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac


./dsh -w -M -r true -m a,b,c,d
case $? in
    0)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac

./dsh -c -r true -m a,b,c,d
case $? in
    0)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac


./dsh -c -M -r true -m a,b,c,d
case $? in
    0)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac

./dsh -c -M -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac


./dsh -c -r ./invalid-exec-file -m a,b,c,d
case $? in
    1)
	echo success
	;;
    *) 
	echo fail
	exit 1;
	;;
esac


