//
// NLBeOSImpl.cpp
//
// Copyright 1997 by Howard Berkey
//

#include "NLEndpoint.h"
#include "NLException.h"


int NLEndpoint::setNonBlock(bool on)
{
	int ltr = (int) on;
	return setOption((NLEndpoint::e_options) SO_NONBLOCK, sol_socket, &ltr, sizeof(ltr));
}
