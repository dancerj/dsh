# Makefile for dancer's shell, copyright 2002 Junichi Uekawa

PACKAGENAME=dsh
OBJECTS=dsh.o linkedlist.o parameter.o

all: $(PACKAGENAME)

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)

$(PACKAGENAME): $(OBJECTS)
	$(CC) -O2 $(OBJECTS) -o $@

%.o: %.c
	$(CC) -O2 -Wall -c -o $@ $<

install: $(PACKAGENAME)
	cp $(PACKAGENAME) $(DESTDIR)/usr/bin
