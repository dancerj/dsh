#!/bin/sh
# check if NEWS is updated
[ $(head -1 ${srcdir}/NEWS | cut -f 1 -d' ') = "${VERSION}" ]
