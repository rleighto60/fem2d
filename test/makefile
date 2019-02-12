#
default: nlbeam.dat rotate.dat srm4seg.dat

%.dat:  %.rexx
	../bin/fem2d $<

clean:
	rm -f *.dat *.ps

