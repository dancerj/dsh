bin_PROGRAMS=dsh
dsh_SOURCES= dsh.c linkedlist.c parameter.c dsh.h linkedlist.h parameter.h gettext.h
dist_man_MANS=dsh.1
sysconf_DATA=dsh.conf
dsh_CFLAGS=-DDSHCONFDIR="\"$(sysconfdir)\"" -DDSH_COMMANDLINE="\"$(bindir)/$(PACKAGE)\"" -DLOCALEDIR="\"$(localedir)\""

check_PROGRAMS=dsh
TESTS=tests/param-f.sh \
	tests/param-f-fail.sh \
	tests/param-m.sh \
	tests/param-unknown.sh  \
	tests/param-i.sh  \
	tests/param-cn4.sh \
	tests/param-gnu-getopt.sh
TESTS_ENVIRONMENT=bindir=$(bindir) \
	LC_ALL=C \
	LANG=C

EXTRA_DIST= config.rpath $(sysconf_DATA) autogen.sh \
	$(TESTS) tests/list.file


# Make debian pristine-source
../@PACKAGE@_@VERSION@.orig.tar.gz: 
	if [ ! -f $@ ]; then $(MAKE) distcheck && dch -v "@VERSION@-1" "New upstream version" && mv @PACKAGE@-@VERSION@.tar.gz ../@PACKAGE@_@VERSION@.orig.tar.gz; \
	else \
	rm -f @PACKAGE@-@VERSION@.tar.gz; \
	fi

debiandist: ../@PACKAGE@_@VERSION@.orig.tar.gz

viper: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz viper2.netfort.gr.jp:public_html/software/downloads
	rm -f @PACKAGE@-@VERSION@.tar.gz

mikilab: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz mikilab.doshisha.ac.jp:parallel-research/software
	rm -f @PACKAGE@-@VERSION@.tar.gz

upload-dist-all: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz viper2.netfort.gr.jp:public_html/software/downloads
	scp @PACKAGE@-@VERSION@.tar.gz mikilab.doshisha.ac.jp:parallel-research/software
	$(MAKE) ../@PACKAGE@_@VERSION@.orig.tar.gz

SUBDIRS = m4 po 

ACLOCAL_AMFLAGS = -I m4
