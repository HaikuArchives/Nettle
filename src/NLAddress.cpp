//
// NLAddress.cpp
//
// Copyright 1997 by Howard Berkey
//

#include "NLAddress.h"
#include "NLException.h"
#include <netdb.h>
#include <stdio.h>


NLAddress::NLAddress(const char *hostname , unsigned short port )
{
	set(hostname, port);
}



NLAddress::NLAddress(struct sockaddr_in &sa)
{
	set(sa);
}



NLAddress::NLAddress(in_addr addr, int port )
{
	set(addr, port);
}


NLAddress::NLAddress(NLAddress::e_addr addr, int port )
{
  in_addr ia;
  ia.s_addr = addr;
  set(ia, port);
}




void NLAddress::p_set(in_addr ia, unsigned short port)
{
	memset(&m_sa, 0, sizeof(m_sa));
	m_sa.sin_family = af_inet;
	m_sa.sin_addr = ia;
	m_sa.sin_port = htons(port);
}



void NLAddress::set(const char *hostname , unsigned short port )
{
	struct hostent *he;
	in_addr ia;

	ia.s_addr = addr_any;

	if(hostname !=  0)
	{
		ia.s_addr = inet_addr(hostname);
		if(ia.s_addr == addr_any || ia.s_addr == (unsigned long) -1)
		{
			he = ::gethostbyname(hostname);
			if(he != 0)
			{
				ia.s_addr = *(int *)he->h_addr_list[0];
			} else {
				char errbuf[256];
				sprintf(errbuf, "::gethostbyname failed for host name %s: %s",
					hostname, strerror(h_errno));
				NL_THROW("NLAddress::set", errbuf);
			}
		}
	}

	p_set(ia, port);
}



void NLAddress::set(struct sockaddr_in &sa)
{
	memcpy(&m_sa, &sa, sizeof(m_sa));
}



void NLAddress::set(in_addr addr, int port )
{
	p_set(addr, port);
}



void NLAddress::set(NLAddress::e_addr addr, int port )
{
	in_addr ia;
	ia.s_addr = addr;
	p_set(ia, port);
}




void NLAddress::get(char *hostname , unsigned short *port ) const
{
	char *abuf;
	if(port != 0)
		*port = ntohs(m_sa.sin_port);
	if(hostname != 0)
	{
		abuf = inet_ntoa(m_sa.sin_addr);
		if(abuf != 0)
		{
			strcpy(hostname, abuf);
		}
	}
}



void NLAddress::get(struct sockaddr_in &sa) const
{
	memcpy(&sa, &m_sa, sizeof(sa));
}



void NLAddress::get(in_addr &addr, unsigned short *port ) const
{
	if(port != 0)
		*port = ntohs(m_sa.sin_port);

	addr = m_sa.sin_addr;
}






