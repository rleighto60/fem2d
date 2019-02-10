# 
CC = gcc -O3 -Wall -fomit-frame-pointer
LIBS = -lm

OFILES = equ2d.o iff.o error.o freemem.o

../../bin/equ2d:      $(OFILES)
		$(CC) -o ../../bin/equ2d $(OFILES) $(LIBS)

equ2d.o:   ../common/header.h ../common/shapes.h equ2d.c
		$(CC) -c -DEQU2D -I../common -o equ2d.o equ2d.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -DEQU2D -I../common -o iff.o ../common/iff.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -DEQU2D -I../common -o error.o ../common/error.c

freemem.o: ../common/header.h ../common/freemem.c
		$(CC) -c -DEQU2D -I../common -o freemem.o ../common/freemem.c

clean:
		rm -f *.o
