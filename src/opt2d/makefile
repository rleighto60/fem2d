# 
CC = gcc -O3 -Wall -fomit-frame-pointer
LIBS = -lm

OFILES = opt2d.o iff.o error.o freemem.o

../../bin/opt2d:      $(OFILES)
		$(CC) -o ../../bin/opt2d $(OFILES) $(LIBS)

opt2d.o:   ../common/header.h ../common/shapes.h opt2d.c
		$(CC) -c -DOPT2D -I../common -o opt2d.o opt2d.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -DOPT2D -I../common -o iff.o ../common/iff.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -DOPT2D -I../common -o error.o ../common/error.c

freemem.o: ../common/header.h ../common/freemem.c
		$(CC) -c -DOPT2D -I../common -o freemem.o ../common/freemem.c

clean:
		rm *.o
