# Makefile for dancer's shell, copyright 2002 Junichi Uekawa

PACKAGENAME=dsh
OBJECTS=dsh.o linkedlist.o parameter.o
CFLAGS=-O2 -g -Wall

all: $(PACKAGENAME) 

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)
	rm -f *.o 

$(PACKAGENAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -ldshconfig

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

install: $(PACKAGENAME)
	install -d -m 755 $(DESTDIR)/usr/bin
	$(LIBTOOL) install -m 755 $(PACKAGENAME) $(DESTDIR)/usr/bin
