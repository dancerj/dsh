#! /bin/sh
echo test to check that unknown parameter checking is right.
set -e
if ./dsh -recho --unknown-parameter; then
    exit 1;
fi

if ./dsh -recho -u; then
    exit 1;
fi
