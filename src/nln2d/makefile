# 
CC = gcc -O3 -Wall -fomit-frame-pointer
LIBS = -lm

OFILES= nln2d.o formkfb.o solve.o elem.o bcint.o shape.o grad.o \
        force.o getmat.o iff.o eval.o check.o error.o freemem.o

../../bin/nln2d:	$(OFILES)
		$(CC) -o ../../bin/nln2d $(OFILES) $(LIBS)

nln2d.o:  ../common/header.h ../common/shapes.h nln2d.c
		$(CC) -c -DNLN2D -I../common -o nln2d.o nln2d.c

formkfb.o: ../common/header.h ../common/shapes.h formkfb.c
		$(CC) -c -DNLN2D -I../common formkfb.c

solve.o:   ../common/header.h ../common/shapes.h solve.c
		$(CC) -c -DNLN2D -I../common solve.c

elem.o:    ../common/header.h ../common/shapes.h elem.c
		$(CC) -c -DNLN2D -I../common elem.c

bcint.o:   ../common/header.h ../common/shapes.h bcint.c
		$(CC) -c -DNLN2D -I../common bcint.c

shape.o:   ../common/header.h ../common/shapes.h ../common/shape.c
		$(CC) -c -DNLN2D -I../common -o shape.o ../common/shape.c

grad.o:    ../common/header.h ../common/shapes.h ../common/grad.c
		$(CC) -c -DNLN2D -I../common -o grad.o ../common/grad.c

force.o:   ../common/header.h ../common/shapes.h ../common/force.c
		$(CC) -c -DNLN2D -I../common -o force.o ../common/force.c

getmat.o:  ../common/header.h ../common/shapes.h ../common/getmat.c
		$(CC) -c -DNLN2D -I../common -o getmat.o ../common/getmat.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -DNLN2D -I../common -o iff.o ../common/iff.c

eval.o:    ../common/header.h ../common/shapes.h ../common/eval.c
		$(CC) -c -DNLN2D -I../common -o eval.o ../common/eval.c

check.o:   ../common/header.h ../common/shapes.h ../common/check.c
		$(CC) -c -DNLN2D -I../common -o check.o ../common/check.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -DNLN2D -I../common -o error.o ../common/error.c

freemem.o: ../common/header.h ../common/freemem.c
		$(CC) -c -DNLN2D -I../common -o freemem.o ../common/freemem.c

clean:
		rm *.o
