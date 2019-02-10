# 
CC = gcc -O3 -Wall -fomit-frame-pointer
LIBS = -lregina -lm

OFILES= fem2d.o commands.o shape.o grad.o force.o getmat.o iff.o \
	iffgroup.o eval.o check.o error.o plot.o parse.o freemem.o

../../bin/fem2d:	$(OFILES)
		$(CC) -o ../../bin/fem2d $(OFILES) $(LIBS)

fem2d.o:  ../common/header.h ../common/shapes.h fem2d.c fem2d.h commands.h hash.h
		$(CC) -c -I../common -o fem2d.o fem2d.c

../hash/hash: ../hash/hash.c
		$(CC) -o ../hash/hash ../hash/hash.c

hash.h: ../hash/hash functions
		../hash/hash functions > hash.h

commands.o: ../common/header.h ../common/shapes.h commands.c commands.h
		$(CC) -c -I../common commands.c

shape.o:   ../common/header.h ../common/shapes.h ../common/shape.c
		$(CC) -c -I../common -o shape.o ../common/shape.c

grad.o:    ../common/header.h ../common/shapes.h ../common/grad.c
		$(CC) -c -I../common -o grad.o ../common/grad.c

force.o:   ../common/header.h ../common/shapes.h ../common/force.c
		$(CC) -c -I../common -o force.o ../common/force.c

getmat.o:  ../common/header.h ../common/shapes.h ../common/getmat.c
		$(CC) -c -I../common -o getmat.o ../common/getmat.c

iff.o:     ../common/header.h ../common/shapes.h ../common/iff.h ../common/iff.c
		$(CC) -c -I../common -o iff.o ../common/iff.c

iffgroup.o: ../common/header.h ../common/shapes.h ../common/iff.h iffgroup.c
		$(CC) -c -I../common iffgroup.c

eval.o:    ../common/header.h ../common/shapes.h ../common/eval.c
		$(CC) -c -I../common -o eval.o ../common/eval.c

check.o:   ../common/header.h ../common/shapes.h ../common/check.c
		$(CC) -c -I../common -o check.o ../common/check.c

plot.o:    ../common/header.h ../common/shapes.h plot.c plot.h
		$(CC) -c -I../common plot.c

error.o:   ../common/header.h ../common/error.c
		$(CC) -c -I../common -o error.o ../common/error.c

parse.o:   ../common/parse.c
		$(CC) -c -I../common -o parse.o ../common/parse.c

freemem.o: ../common/header.h ../common/shapes.h ../common/freemem.c
		$(CC) -c -I../common -o freemem.o ../common/freemem.c

clean:
		rm *.o hash.h
