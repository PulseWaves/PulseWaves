all:
	cd src && make
	cd example && make
#	cd test && make

clean:
	cd src && make clean
	cd example && make clean
#	cd test && make clean
	rm -f lib/*

clobber:
	cd src && make clobber
	cd example && make clobber
#	cd test && make clobber
	rm -f lib/*
