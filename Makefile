# Makefile for dancer's shell, copyright 2002 Junichi Uekawa

PACKAGENAME=dsh
OBJECTS=dsh.lo linkedlist.lo parameter.lo libdshconfig.la
SONAME=0:0:0
LIBTOOL=libtool
CFLAGS=-O2 -g -Wall

all: $(PACKAGENAME) test-apps

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)
	$(LIBTOOL) --mode=clean rm -f *.lo *.la 

$(PACKAGENAME): $(OBJECTS)
	$(LIBTOOL) $(CC) --mode=link $(CFLAGS) $(OBJECTS) -o $@

test-apps: test-dshconfig

test-dshconfig: test-dshconfig.lo libdshconfig.la
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) $^ -o $@

libdshconfig.la: libdshconfig.lo
	# specifying -rpath /usr/lib should not set rpath, but is required for libtool to make shared lib.
	$(LIBTOOL) --mode=link $(CC) -rpath /usr/lib -export-dynamic -version-info $(SONAME) $^ -o $@ $(CFLAGS) 

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) -c -o $@ $< $(CFLAGS) 

install: $(PACKAGENAME)
	install -d -m 755 $(DESTDIR)/usr/bin
	install -d -m 755 $(DESTDIR)/usr/lib
	install -d -m 755 $(DESTDIR)/usr/include
	$(LIBTOOL) install -m 755 $(PACKAGENAME) $(DESTDIR)/usr/bin
	$(LIBTOOL) install -m 644 libdshconfig.la $(DESTDIR)/usr/lib
	install -m 644 libdshconfig.h $(DESTDIR)/usr/include
