# Makefile for dancer's shell, copyright 2002 Junichi Uekawa

PACKAGENAME=dsh
OBJECTS=dsh.o linkedlist.o parameter.o
SONAME=0:0:0
LIBTOOL=libtool

all: $(PACKAGENAME) test-apps

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)
	$(LIBTOOL) --mode=clean rm -f *.lo *.la 

$(PACKAGENAME): $(OBJECTS)
	$(CC) -O2 $(OBJECTS) -o $@

test-apps: test-dshconfig

test-dshconfig: test-dshconfig.lo libdshconfig.la
	$(LIBTOOL) --mode=link $(CC) -O2 $^ -o $@

libdshconfig.la: libdshconfig.lo
	# specifying -rpath /usr/lib should not set rpath, but is required for libtool to make shared lib.
	$(LIBTOOL) --mode=link $(CC) -rpath /usr/lib -export-dynamic -version-info $(SONAME) $^ -o $@ 

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) -g -O2 -Wall -c -o $@ $<

install: $(PACKAGENAME)
	install -d -m 755 $(PACKAGENAME) $(DESTDIR)/usr/bin
	install -m 755 $(PACKAGENAME) $(DESTDIR)/usr/bin
