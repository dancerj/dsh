PACKAGENAME=dsh

all: $(PACKAGENAME)

clean:
	rm -f *~ *.bak *.o $(PACKAGENAME)

$(PACKAGENAME): $(PACKAGENAME).o
	gcc -g $< -o $@ 

%.o: %.c
	gcc -g -Wall -c -o $@ $<

install: $(PACKAGENAME)
	cp $(PACKAGENAME) $(DESTDIR)/usr/bin
