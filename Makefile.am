bin_PROGRAMS=dsh
dsh_SOURCES= dsh.c linkedlist.c parameter.c dsh.h linkedlist.h parameter.h
dsh_LDFLAGS= -ldshconfig

# Make debian pristine-source
../@PACKAGE@_@VERSION@.orig.tar.gz: 
	if [ ! -f $@ ]; then $(MAKE) distcheck && dch -v "@VERSION@-1" "New upstream version" && mv @PACKAGE@-@VERSION@.tar.gz ../@PACKAGE@_@VERSION@.orig.tar.gz; \
	else \
	rm -f @PACKAGE@-@VERSION@.tar.gz; \
	fi

debiandist: ../@PACKAGE@_@VERSION@.orig.tar.gz

decaton: distcheck
	scp @PACKAGE@-@VERSION@.tar.gz decaton.netfort.gr.jp:public_html/software/downloads
	rm -f @PACKAGE@-@VERSION@.tar.gz
