#!/bin/sh
# check if NEWS is updated
set -e 

[ $(head -1 ${srcdir}/NEWS | cut -f 1 -d' ') = "${VERSION}" ]
