#! /bin/sh
# script to distributedly copy files.

PWD=`pwd`

cat ${tempfile} | @bindir@/dsh "$@" -ic "test -x @libexecdir@/dcpd && ( cd ${PWD}; cat > .tmp.f ;  tar xfz tmp.f; true) || @libexecdir@/dcpd ${PWD} 0"