#!/bin/bash
# autogenerate script

libtoolize -c --force && aclocal -I m4 && autoheader && automake --foreign -a -c && autoconf

