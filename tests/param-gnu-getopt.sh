#! /bin/sh
# check if things are working in a POSIX way, and ensure that GNU getopt workaround is working.
set -e

[ "`./dsh -Mm hoge,test -r echo -c test -m other | sort`" = "\
hoge: hoge test -m other
test: test test -m other" ]