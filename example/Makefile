# makefile for examples
#
#COPTS    = -g -Wall
COPTS     = -O3 -Wall
COMPILER  = g++
LINKER  = g++
#BITS     = -64

#LIBS     = -L/usr/lib32
#LIBS     = -L/usr/lib64
INCLUDE  = -I/usr/include

PULSEWAVES_LIBS     = -L../lib
PULSEWAVES_INCLUDE  = -I../inc

all: exampleLIB

exampleLIB: exampleLIB.o
	${LINKER} ${BITS} ${COPTS} exampleLIB.o -lpulsewaves -o $@ ${LIBS} ${PULSEWAVES_LIBS} $(INCLUDE) $(PULSEWAVES_INCLUDE)

.cpp.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(PULSEWAVES_INCLUDE) $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(PULSEWAVES_INCLUDE) $< -o $@

clean:
	rm -rf *.o
	rm -rf exampleLIB

clobber:
	rm -rf *.o
	rm -rf exampleLIB
	rm -rf *~
