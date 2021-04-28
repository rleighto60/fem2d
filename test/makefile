#
default: nlbeam.dat plate.dat rotate.dat srm4seg.dat

%.dat:  %.rexx
	PATH=../bin fem2d $<

clean:
	rm -f *.dat *.ps

