bin_PROGRAMS=dsh
dsh_SOURCES= dsh.c linkedlist.c parameter.c
dsh_LDFLAGS= -ldshconfig

# Make debian pristine-source
../@PACKAGE@_@VERSION@.orig.tar.gz: dist
	mv @PACKAGE@-@VERSION@.tar.gz ../@PACKAGE@_@VERSION@.orig.tar.gz

debiandist: ../@PACKAGE@_@VERSION@.orig.tar.gz

decaton: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz decaton.netfort.gr.jp:public_html/software/downloads
	rm -f @PACKAGE@-@VERSION@.tar.gz
