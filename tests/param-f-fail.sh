#!/bin/sh
# test to check that -f parameter fails properly.
set -e
if ./dsh -recho -M -f ${srcdir}/tests/nonexistent.file ; then
    exit 1
else
    exit 0
fi

