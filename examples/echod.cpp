//
// echod.cpp
// 
// Example udp echo daemon using the net classes
//
// Note that on Solaris inetd is already bound to port 7 
// so this will fail. Remove the NLAddress::udpEcho below and
// it should work fine.
//

#include "NLEndpoint.h"
#include "NLException.h"


int main(int, char**)
{
	char buf[8192], addrstr[128];
	NLEndpoint comm(NLEndpoint::datagram);
	NLAddress addr;	
	int rc;
	unsigned short port;
	
	try {
		comm.bind(NLAddress::udpEcho);
		addr = comm.getAddr();
		addr.get(addrstr, &port);
		
		printf("Echo daemon up at %s port %d\n", addrstr, port);
		
		while(1)
		{
			rc = comm.recvfrom(buf, sizeof(buf), addr);
			comm.sendto(buf, rc, addr);
			addr.get(addrstr);
			printf("Got %d bytes from %s\n", rc, addrstr);;
		}
	}
	
	catch (NLException &ex) {
		char s[2048];
		ex.print(s);
		fprintf(stderr, "%s\n", s);
	}
	return -1;	
}
