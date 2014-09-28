#
#  Makefile for nettle
#

include ./make.incl

all: libs examples

libs::
	(cd src; make)

examples:: libs 
	(cd examples; make)

clean:
	(cd src; make clean)
	(cd examples; make clean)

clobber:
	(cd src; make clobber)
	(cd examples; make clobber)
