#! /bin/sh
# test for comment-handling in machinelist
[ "`./dsh -M -f ${srcdir}/tests/machinelist-comment.txt -recho | sort `" = "\
hostname: hostname
this#notcomment: this#notcomment"  ];
