#! /bin/sh
# test for machinelist ordering.
[ "`./dsh -M -m one,two -m three -f ${srcdir}/tests/machinelist-comment.txt -m four -recho `" = "\
one: one
two: two
three: three
hostname: hostname
this#notcomment: this#notcomment
four: four"  ];
