#!/bin/bash
set -e 
echo Check redirect and pipe output will do the same thing, even when there are so many hosts to process.
nmachines=1342
machines=$(echo $(seq 1 $nmachines ) | sed 's/ /,/g')

t1=$(tempfile)
t2=$(tempfile)

echo expecting: $nmachines
echo For pipe:
./dsh -m ${machines} -M -F 20 -r./tests/sleepyshell hostname 2> /dev/null | sort > "$t1" ; wc -l "$t1"
echo For redirect:
./dsh -m ${machines} -M -F 20 -r./tests/sleepyshell hostname 2> /dev/null > "$t2" ; wc -l "$t2"

[ $(cat "$t1"| wc -l ) = "$nmachines" ]
[ $(cat "$t2"| wc -l ) = "$nmachines" ]

rm "$t1" "$t2"
