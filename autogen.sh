#!/bin/bash
# autogenerate script

libtoolize -c --force && aclocal && autoheader && automake --foreign -a -c && autoconf && ./configure

