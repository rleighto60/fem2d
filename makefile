#
default:
	mkdir -p bin
	cd src; make
	cd test; make
	cd doc; make

clean:
	cd src; make clean
	cd test; make clean
	cd doc; make clean
	rm -rf bin
