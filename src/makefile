#
#  Makefile for nettle
#


INCLUDE=-I../include

LIBS=../lib/libnettle.so

include ../make.incl

OBJS=NLDebug.o NLPacket.o NLEndpoint.o NLAddress.o $(PLATOBJS)

all: libs

libs: $(OBJS)
	rm -f $(LIBS)
	mkdir -p ../lib
	$(LINK) $(LIBS) $(OBJS)

clean:
	rm -f $(LIBS) $(OBJS)

clobber: clean
	rm -f *~ *.xSYM *.dbg
