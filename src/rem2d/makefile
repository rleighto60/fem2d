# 
CC = gcc -O3 -Wall -fomit-frame-pointer
LIBS = -lm

OFILES = rem2d.o iff.o error.o freemem.o

../../bin/rem2d:      $(OFILES)
		$(CC) -o ../../bin/rem2d $(OFILES) $(LIBS)

rem2d.o:   ../common/header.h ../common/shapes.h rem2d.c
		$(CC) -c -DREM2D -I../common -o rem2d.o rem2d.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -DREM2D -I../common -o iff.o ../common/iff.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -DREM2D -I../common -o error.o ../common/error.c

freemem.o: ../common/header.h ../common/freemem.c
		$(CC) -c -DREM2D -I../common -o freemem.o ../common/freemem.c

clean:
		rm -f *.o
