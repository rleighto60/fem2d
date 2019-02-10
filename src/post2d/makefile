# 
CC = gcc -O3 -DUNIX -Wall -fomit-frame-pointer
LIBS = -lm

OFILES= post2d.o post.o shape.o grad.o force.o getmat.o iff.o eval.o \
        check.o error.o freemem.o

../../bin/post2d:	$(OFILES)
		$(CC) -o ../../bin/post2d $(OFILES) $(LIBS)

post2d.o: ../common/header.h ../common/shapes.h post2d.c
		$(CC) -c -DPOST2D -I../common -o post2d.o post2d.c

post.o:    ../common/header.h ../common/shapes.h post.c
		$(CC) -c -DPOST2D -I../common post.c

shape.o:   ../common/header.h ../common/shapes.h ../common/shape.c
		$(CC) -c -DPOST2D -I../common -o shape.o ../common/shape.c

grad.o:    ../common/header.h ../common/shapes.h ../common/grad.c
		$(CC) -c -DPOST2D -I../common -o grad.o ../common/grad.c

force.o:   ../common/header.h ../common/shapes.h ../common/force.c
		$(CC) -c -DPOST2D -I../common -o force.o ../common/force.c

getmat.o:  ../common/header.h ../common/shapes.h ../common/getmat.c
		$(CC) -c -DPOST2D -I../common -o getmat.o ../common/getmat.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -DPOST2D -I../common -o iff.o ../common/iff.c

eval.o:    ../common/header.h ../common/shapes.h ../common/eval.c
		$(CC) -c -DPOST2D -I../common -o eval.o ../common/eval.c

check.o:   ../common/header.h ../common/shapes.h ../common/check.c
		$(CC) -c -DPOST2D -I../common -o check.o ../common/check.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -DPOST2D -I../common -o error.o ../common/error.c

freemem.o: ../common/header.h ../common/freemem.c
		$(CC) -c -DPOST2D -I../common -o freemem.o ../common/freemem.c

clean:
		rm -f *.o
