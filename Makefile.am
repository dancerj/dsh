bin_PROGRAMS=dsh
dsh_SOURCES= dsh.c linkedlist.c parameter.c dsh.h linkedlist.h parameter.h gettext.h
original_man=dsh.1 dsh.conf.5 
man_MANS=build-man/dsh.1 build-man/dsh.conf.5
translated_man=dsh.conf.ja.5 dsh.ja.1
sysconf_DATA=dsh.conf
dsh_CFLAGS=-DDSHCONFDIR="\"$(sysconfdir)\"" -DDSH_COMMANDLINE="\"$(bindir)/$(PACKAGE)\"" -DLOCALEDIR="\"$(localedir)\""

# space-delimited list of languages the manual pages are provided.
#  dsh.LL.1 and dsh.conf.LL.5
man_languages=ja

check_PROGRAMS=dsh
TESTS=tests/param-f.sh \
	tests/param-f-fail.sh \
	tests/param-f-with-space.sh \
	tests/param-m.sh \
	tests/param-unknown.sh  \
	tests/param-i.sh  \
	tests/param-cn4.sh \
	tests/param-b-0.sh \
	tests/param-r-invalid.sh \
	tests/param-F-invalid.sh \
	tests/param-F-forklimit.sh \
	tests/param-gnu-getopt.sh \
	tests/news-okay.sh \
	tests/segv-catcher.sh \
	tests/test-bufferoverflow.sh \
	tests/machinelist-comment-test.sh \
	tests/machinelist-order.sh

TESTS_ENVIRONMENT=bindir=$(bindir) \
	LC_ALL=C \
	LANG=C \
	VERSION=@VERSION@ 
EXTRA_DIST= config.rpath $(sysconf_DATA) autogen.sh \
	$(TESTS) tests/list.file tests/list.space.file \
	tests/machinelist-comment.txt \
	$(translated_man) $(original_man) \
	dsh.spec

${srcdir}/dsh.1.ja.po: ${srcdir}/dsh.1
	po4a-updatepo -f man -m dsh.1 -p dsh.1.ja.po 

${srcdir}/dsh.ja.1: ${srcdir}/dsh.1.ja.po
	po4a-translate -f man -m dsh.1 -p dsh.1.ja.po -L euc-jp -l dsh.ja.1

install-data-local: 
	for LANGS in $(man_languages) ; do \
		mkdir -p $(DESTDIR)$(mandir)/$$LANGS/man1 ;\
		mkdir -p $(DESTDIR)$(mandir)/$$LANGS/man5 ;\
		sed -e 's,[@]sysconfdir[@],$(sysconfdir),' < ${srcdir}/dsh.$$LANGS.1 \
			> $(DESTDIR)$(mandir)/$$LANGS/man1/dsh.1 ; \
		sed -e 's,[@]sysconfdir[@],$(sysconfdir),' < ${srcdir}/dsh.conf.$$LANGS.5 \
			> $(DESTDIR)$(mandir)/$$LANGS/man5/dsh.conf.5 ; \
	done

uninstall-local: 
	for LANGS in $(man_languages) ; do \
		rm -f $(DESTDIR)$(mandir)/$$LANGS/man1/dsh.1 \
			$(DESTDIR)$(mandir)/$$LANGS/man5/dsh.conf.5 ; \
	done

build-man:
	mkdir build-man

build-man/dsh.1: ${srcdir}/dsh.1 build-man
	sed -e 's,[@]sysconfdir[@],$(sysconfdir),' < ${srcdir}/dsh.1 > $@

build-man/dsh.conf.5: ${srcdir}/dsh.conf.5 build-man
	sed -e 's,[@]sysconfdir[@],$(sysconfdir),' < ${srcdir}/dsh.conf.5 > $@

clean-local:
	-rm -f $(man_MANS)
	-rmdir build-man
	-rm -r doc

# Make debian pristine-source
../@PACKAGE@_@VERSION@.orig.tar.gz: 
	if [ ! -f $@ ]; then $(MAKE) distcheck && dch -v "@VERSION@-1" "New upstream version" && mv @PACKAGE@-@VERSION@.tar.gz ../@PACKAGE@_@VERSION@.orig.tar.gz; \
	else \
	rm -f @PACKAGE@-@VERSION@.tar.gz; \
	fi

debiandist: ../@PACKAGE@_@VERSION@.orig.tar.gz

viper: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz aegis.netfort.gr.jp:public_html/software/downloads
	rm -f @PACKAGE@-@VERSION@.tar.gz

upload-dist-all: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz aegis.netfort.gr.jp:public_html/software/downloads
	$(MAKE) ../@PACKAGE@_@VERSION@.orig.tar.gz

SUBDIRS = m4 po 

ACLOCAL_AMFLAGS = -I m4

doc: ${srcdir}/*.[ch]
	doc++ --dir doc *.[ch]

