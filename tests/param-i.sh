#! /bin/sh
# test whether the -i option works.

[ "`echo test | ./dsh -Mcirecho -ma,b,c | sort`" = "\
a: a
b: b
c: c" ]
