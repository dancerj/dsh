#!/bin/bash
# autogenerate script

libtoolize -c && aclocal && autoheader && automake --foreign -a -c && autoconf && ./configure

