# Makefile for dancer's shell, copyright 2002 Junichi Uekawa

PACKAGENAME=dsh
OBJECTS=dsh.o linkedlist.o parameter.o

all: $(PACKAGENAME) test-apps

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)

$(PACKAGENAME): $(OBJECTS)
	$(CC) -O2 $(OBJECTS) -o $@


test-apps: test-dshconfig

test-dshconfig: test-dshconfig.o libdshconfig.o
	$(CC) -O2 $^ -o $@

%.o: %.c
	$(CC) -g -O2 -Wall -c -o $@ $<

install: $(PACKAGENAME)
	cp $(PACKAGENAME) $(DESTDIR)/usr/bin
