#!/bin/bash
set -e 
echo Check redirect and pipe output will do the same thing, even when there are so many hosts to process.
nmachines=200
machines=$(echo $(seq 1 $nmachines ) | sed 's/ /,/g')

t1=$(tempfile)
t2=$(tempfile)

echo expecting: $nmachines
echo For pipe:
./dsh -m ${machines} -M -F 20 -r./tests/sleepyshell hostname | sort > "$t1" ; wc -l "$t1"
cat "$t1" | sort -n | cut -d: -f1 | awk 'BEGIN{i=1} {if($1 != i) {print i" missing"; i++} i++}'
echo For redirect:
./dsh -m ${machines} -M -F 20 -r./tests/sleepyshell hostname > "$t2" ; wc -l "$t2"
cat "$t2" | sort -n | cut -d: -f1 | awk 'BEGIN{i=1} {if($1 != i) {print i" missing"; i++} i++}'

[ $(cat "$t1"| wc -l ) = "$nmachines" ]
[ $(cat "$t2"| wc -l ) = "$nmachines" ]

rm "$t1" "$t2"

# Above code is shorter than ideal. The following is too slow, but
# this is more reliable.

# ./dsh -m $(echo $(seq 1 3000 ) | sed 's/ /,/g')  -M -F 20 -r./tests/sleepyshell hostname  > "/tmp/test" ; wc -l "/tmp/test"

# cat  | sort -n | cut -d: -f1 | awk 'BEGIN{i=1} {if($1 != i) {print i" missing"; i++} i++}'
