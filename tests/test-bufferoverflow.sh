#! /bin/sh

# check if dsh segfaults with an exceedingly long HOME variable.

HOME=/this/is/absurdly/long/and/it/is/very/very/bad/for/your/health
HOME=${HOME}${HOME}${HOME}${HOME}${HOME}
HOME=${HOME}${HOME}${HOME}${HOME}${HOME}
HOME=${HOME}${HOME}${HOME}${HOME}${HOME}
HOME=${HOME}${HOME}${HOME}${HOME}${HOME} ./dsh -m l -r echo w
