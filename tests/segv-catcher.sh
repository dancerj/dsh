#!/bin/sh
# test whether the assertion is fixed.

set -e

echo test | ./dsh -Mcir echo -ma,b,c

