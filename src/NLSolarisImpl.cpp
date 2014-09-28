//
// NLSolarisImpl.cpp
//
// Copyright 1997 by Howard Berkey
//

#include "NLEndpoint.h"
#include "NLException.h"
#include <unistd.h>
#include <sys/filio.h>

extern "C" int closesocket(int fd)
{
	return close(fd);
}

int NLEndpoint::setNonBlock(bool on)
{
	int ltr = (int) on;
	if (ioctl (m_socket, FIONBIO, (char *)&ltr) == -1)
		return -1;
	return 0;
}
