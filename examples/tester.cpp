//
// tester.cpp
//
// Simple UDP echo client 
//
// Copyright 1997 by Howard Berkey
//

#include "NLEndpoint.h"
#include "NLException.h"


int main(int argc, char**argv)
{
	NLEndpoint comm(NLEndpoint::datagram);
	char *sendstr = "Hello!!!";
	char buf[1024];
	int rc;
	
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s host\n");
		return(-1);
	}
	
	try 
	{
		NLAddress addr;
		addr.set(argv[1], NLAddress::udpEcho);
		
		rc = comm.sendto(sendstr, strlen(sendstr) + 1, addr);
		printf("Sent: %s\n", sendstr);
		comm.recvfrom(buf, rc, addr);
		printf("Received: %s\n", buf);
		
		NLPacket pack;
		
		pack.insert("and again!!!");
		rc = comm.sendto(pack, addr);
		printf("Sent: %s\n", (char *) pack.getData());
		comm.recvfrom(buf, rc, addr);
		printf("Received: %s\n", buf);
	}
	
	catch (NLException &ex)
	{
		char s[2048];
		ex.print(s);
		fprintf(stderr, "%s\n", s);
	}
	return 0;
}

