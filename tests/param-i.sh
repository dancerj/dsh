#! /bin/sh
# test whether the -i option works.

set -e 

echo test1
[ "`echo test | ./dsh -Mcir echo -ma,b,c $(cat) | sort`" = "\
a: a test
b: b test
c: c test" ]

echo test2
[ "`echo test | ./dsh -Mcir echo -ma,b,c | sort`" = "\
a: a
b: b
c: c" ]


