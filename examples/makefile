#
# Makefile for nettle
#

INCLUDE=-I. -I../include
include ../make.incl
LDFLAGS= -L../lib

LIBS=../lib/libnettle.so

all: tester echod server

tester: $(LIBS) tester.o
	$(CPP) $(LDFLAGS)   tester.o $(LINKLIBS) -o  tester

echod: $(LIBS) echod.o
	$(CPP) $(LDFLAGS)   echod.o $(LINKLIBS) -o  echod

server: $(LIBS) server.o
	$(CPP) $(LDFLAGS)   server.o $(LINKLIBS) -o  server

clean: 
	rm -f *.o

clobber: clean
	rm -f echod tester server *SYM *.dbg
