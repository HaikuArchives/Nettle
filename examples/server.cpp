//
// server.cpp
//
// Simple TCP-based server 
//
// Copyright 1997 by Howard Berkey
//

#include "NLEndpoint.h"
#include "NLException.h"


int main(int, char**)
{
	char addrstr[128], *goaway = "\n\nGo away!\n\n";
	NLEndpoint comm, *rep;
	NLAddress addr;	
	unsigned short port;
	
	try {
		comm.bind();
		comm.listen();
		comm.setReuseAddr();		
		addr = comm.getAddr();
		addr.get(addrstr, &port);
		printf("Grumpy server up at %s port %d\n", addrstr, port);

		while(1)
		{
			rep = comm.accept();
			if(rep != 0)
			  {
			    rep->send(goaway, strlen(goaway));
			    addr = rep->getPeer();
			    addr.get(addrstr, &port);
				printf("told %s:%d to go away!\n", addrstr, port);
			    delete rep;
			  }
		}
	}
	
	catch (NLException &ex) {
		char s[2048];
		ex.print(s);
		fprintf(stderr, "%s\n", s);
	}
	return -1;	
}
