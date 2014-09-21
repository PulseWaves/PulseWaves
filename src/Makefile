# makefile for liblas.a
#
#COPTS    = -g -Wall
COMPILER  = g++
AR  = ar rc
#BITS     = -64

INCLUDE  = -I/usr/include/ -I../inc -I.

OBJ_PULSEWAVES_STATIC = pulseheader.o pulsevlrs.o pulsedescriptor.o pulsepulse.o waveswaves.o pulsereader.o pulsewriter.o pulsereadermerged.o pulsereader_pls.o pulsereader_gcw.o pulsereader_lgw.o pulsereader_csd.o pulsewriter_pls.o pulsewriter_txt.o pulsereadwaves_raw.o pulsereadwaves_compressed.o pulsewritewaves_raw.o pulsewritewaves_compressed.o pulseutility.o pulsefilter.o pulsetransform.o pulsezip.o pulsereadpulse.o pulsereaditemcompressed_v1.o pulsewritepulse.o pulsewriteitemcompressed_v1.o integercompressor.o arithmeticdecoder.o arithmeticencoder.o arithmeticmodel.o

staticflags:
	COPTS = -O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED

sharedflags:
	COPTS = -O3 -Wall -Wno-deprecated -DNDEBUG -DUNORDERED -DPULSEWAVES_DLL

OBJ_PULSEWAVES_SHARED = pulsewaves_dll.o pulseheader.o pulsevlrs.o pulsedescriptor.o pulsepulse.o waveswaves.o pulsereader.o pulsewriter.o pulsereadermerged.o pulsereader_pls.o pulsewriter_pls.o pulsewriter_txt.o pulsereadwaves_raw.o pulsereadwaves_compressed.o pulsewritewaves_raw.o pulsewritewaves_compressed.o pulseutility.o pulsefilter.o pulsetransform.o pulsezip.o pulsereadpulse.o pulsereaditemcompressed_v1.o pulsewritepulse.o pulsewriteitemcompressed_v1.o integercompressor.o arithmeticdecoder.o arithmeticencoder.o arithmeticmodel.o

static: staticflags libpulsewaves.a

shared: sharedflags libpulsewaves.so

libpulsewaves.a: ${OBJ_PULSEWAVES_STATIC}
	$(AR) $@ ${OBJ_PULSEWAVES_STATIC}
	cp -p $@ ../lib

libpulsewaves.so: ${OBJ_PULSEWAVES_SHARED}
	$(AR) -shared -o $@ ${OBJ_PULSEWAVES_SHARED}
	cp -p $@ ../lib

.cpp.o:
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

clean:
	rm -rf libpulsewaves.a
	rm -rf *.o

clobber:
	rm -rf libpulsewaves.a
	rm -rf *.o
	rm -rf *~
