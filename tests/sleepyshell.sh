#!/bin/bash
# a fake remote shell command that sleeps for random seconds and echoes back its parameter, used for testing.
# used in pipe-vs-redirect.sh

t=$(( RANDOM % 2 + 1 ))
sleep $t
echo "$@"




