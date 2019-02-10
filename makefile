#
default:
	mkdir -p bin
	cd src; make
	cd doc; make

clean:
	cd src; make clean
	cd doc; make clean
	rm -rf bin

install:
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/doc/fem2d
	mkdir -p $(DESTDIR)/usr/share/info
	cp bin/* $(DESTDIR)/usr/bin/
	cp doc/*.pdf $(DESTDIR)/usr/share/doc/fem2d/
	tar -czf $(DESTDIR)/usr/share/doc/fem2d/test.tgz rexx test
	cp doc/*.info $(DESTDIR)/usr/share/info/
