/
// NLLinuxImpl.cpp
//
// Copyright 1997 by Howard Berkey
//



#include "NLEndpoint.h"
#include "NLException.h"
#include <unistd.h>
#include <sys/fcntl.h>


extern "C" int closesocket(int fd)
{
	return close(fd);
}


int NLEndpoint::setNonBlock(bool on)
{
 	int ltr = (int)( on ? O_NONBLOCK : 0);
        return fcntl(sol_socket, F_SETFL, ltr);
}  

