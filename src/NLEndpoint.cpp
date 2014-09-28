//
// NLEndpoint.cpp
//
// Copyright 1997 by Howard Berkey
//

#include "NLEndpoint.h"
#include "NLException.h"
#include "otto_ptr"
#if defined(IRIX)
#include <sys/time.h>
#include <unistd.h>
#endif
#if defined(__HAIKU__)
#include <sys/select.h>
#endif

extern "C" int closesocket(int fd);  // for portability

unsigned long NLEndpoint::s_inval = 0x00000000;
unsigned long NLEndpoint::s_open = 0x00000001;
unsigned long NLEndpoint::s_bound = 0x00000002;
unsigned long NLEndpoint::s_connected = 0x00000004;

void NLEndpoint::p_resetSocket()
{
	char errbuf[256];
	
	close();
	m_socket = ::socket(NLAddress::af_inet, m_proto, 0);
	if(m_socket < 0)
	{
		m_state = s_inval;
		sprintf(errbuf, "::socket() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::p_resetSocket()", errbuf, this);
	}
	m_state |= s_open;
}


NLEndpoint::NLEndpoint(NLEndpoint::e_type proto)
{
	m_state = NLEndpoint::s_inval;
	m_proto = proto;
	p_resetSocket();
	m_timeout = -1;
	m_throwsend = true;
	m_throwrecv = true;
	m_lasterr = 0;
}


NLEndpoint::~NLEndpoint()
{
	close();
}



void NLEndpoint::setProtocol(NLEndpoint::e_type proto)
{
	m_proto = proto;
	p_resetSocket();
}


int NLEndpoint::setOption(NLEndpoint::e_options opt, NLEndpoint::e_optlevel lev, const void *data, unsigned int datasize)
{
	if(m_state == s_inval)
		NL_THROW_ARG("NLEndpoint::setOption", "Socket not opened", this);
		
	return ::setsockopt(m_socket, lev, opt, (const char *) data, datasize);
}



int NLEndpoint::setReuseAddr(bool on)
{
	int ltr = (int) on;
	return setOption(reuseopt, sol_socket, &ltr, sizeof(ltr));
}



void NLEndpoint::close()
{
	if(m_state & s_open)
	{
		closesocket(m_socket);
		m_socket = -1;
	}
	m_state = NLEndpoint::s_inval;

}



void NLEndpoint::bind(const NLAddress &addr)
{
	struct sockaddr_in sa;
	char errbuf[256];	
	int s = sizeof(struct sockaddr_in);
	
	if(m_state != s_open)
		p_resetSocket();
	
	addr.get(sa);
	
	if(::bind(m_socket, (struct sockaddr *) &sa, s) < 0)
	{
		close();
		sprintf(errbuf, "::bind() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::bind()", errbuf, this);	
	}
	
	if(::getsockname(m_socket, (struct sockaddr *) &sa, &s) < 0)
	{
		close();
		sprintf(errbuf, "::getsockname() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::bind()", errbuf, this);	
	}

	if(sa.sin_addr.s_addr == 0)
	{
	    // buggy getsockname

	    char hn[NLAddress::maxhostnamelen];
	    struct hostent *he;

	    gethostname(hn, NLAddress::maxhostnamelen);
	    he = gethostbyname(hn);
	    if(he != 0)
		memcpy(&sa.sin_addr.s_addr, he->h_addr, sizeof(sa.sin_addr.s_addr));
	}
	
	m_addr.set(sa);
	m_state |= s_bound;
}


void NLEndpoint::bind(int port)
{
	NLAddress addr(NLAddress::addr_any, port);
	bind(addr);
}



void NLEndpoint::connect(const NLAddress &addr)
{
	struct sockaddr_in sa;
	char errbuf[256];	
	int s = sizeof(struct sockaddr_in);
	
	if((m_state & s_open) != true)
		p_resetSocket();
	
	addr.get(sa);
	
	if(::connect(m_socket, (struct sockaddr *) &sa, s) < 0)
	{
		close();
		sprintf(errbuf, "::connect() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::connect()", errbuf, this);	
	}
	
	if(::getpeername(m_socket, (struct sockaddr *) &sa, &s) < 0)
	{
		close();
		sprintf(errbuf, "::getpeername() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::connect()", errbuf, this);	
	}
	m_peer.set(sa);
	m_state |= s_connected;
}


void NLEndpoint::connect(const char *addr, int port)
{
	NLAddress naddr(addr, port);
	connect(naddr);
}



void NLEndpoint::listen(int backlog)
{
	char errbuf[256];
	
	if(::listen(m_socket, backlog) < 0)
	{
		close();
		sprintf(errbuf, "::listen() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::listen()", errbuf, this);	
	}
	
}


NLEndpoint *NLEndpoint::clone(NLEndpoint *)
{
	return new NLEndpoint(*this);
}

NLEndpoint *NLEndpoint::accept(long timeout)
{
	NLEndpoint *nep;
	struct sockaddr_in sa;
	int s = sizeof(sa);
	int newsock;
	char errbuf[256];
	
	if(isDataPending(timeout) == false)
		return 0;
	
	if((newsock = ::accept(m_socket, (struct sockaddr *) &sa, &s)) < 0)
	{
		close();
		sprintf(errbuf, "::accept() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::accept()", errbuf, this);	
	}

	nep = clone(this);

	if(nep == 0)
		NL_THROW_ARG("NLEndpoint::accept", "Out of memory", this);
	
	nep->m_peer.set(sa);
	nep->m_socket = newsock;
	nep->m_state = s_open | s_connected;
	
	if(::getsockname(nep->m_socket, (struct sockaddr *) &sa, &s) < 0)
	{
		nep->close();
		delete nep;
		sprintf(errbuf, "::getsockname() failed: %s", strerror(errno));
		NL_THROW_ARG("NLEndpoint::accept()", errbuf, this);	
	}
	
	nep->m_addr.set(sa);
	
	return nep;
}



size_t NLEndpoint::send(const void *buf, size_t size, int flags)
{
	long rc;
	
	rc = ::send(m_socket, (const char *) buf, size, flags);

	if(rc < 0)
	{
		m_lasterr = errno;
		if(m_throwsend == true)
		{
			char errbuf[256];
			sprintf(errbuf, "::send() failed: %s", strerror(errno));
			NL_THROW_ARG("NLEndpoint::send()", errbuf, this);	
		}
	}

	NLDebug::dump((char *) buf, rc, "NLEndpoint sent:");

	return rc;
}


size_t NLEndpoint::send(NLPacket &pack, int flags)
{
	size_t rc;

	rc = send(pack.getData(), pack.getSize(), flags);
	
	return rc;

}



size_t NLEndpoint::recv(void *buf, size_t size, int flags)
{
	long rc;
	
	if(m_timeout >= 0 && isDataPending(m_timeout) == false)
		return 0;
		
	rc = ::recv(m_socket, (char *) buf, size, flags);

	if(rc < 0)
	{
		m_lasterr = errno;
		if(m_throwrecv == true)
		{
			char errbuf[256];
			sprintf(errbuf, "::recv() failed: %s", strerror(errno));
			NL_THROW_ARG("NLEndpoint::recv()", errbuf, this);	
		}
	}
	NLDebug::dump((char *) buf, rc, "NLEndpoint received:");	
	return rc;

}

	

size_t NLEndpoint::sendto(const void *buf, size_t size, const NLAddress &to, int flags)
{
	struct sockaddr_in sa;
	size_t s = sizeof(sa);
	long rc;
	
	to.get(sa);
	rc = ::sendto(m_socket, (const char *) buf, size, flags, (struct sockaddr *) &sa, s);
	if(rc < 0)
	{
		m_lasterr = errno;
		if(m_throwsend == true)
		{
			char errbuf[256];
			sprintf(errbuf, "::sendto() failed: %s", strerror(errno));
			NL_THROW_ARG("NLEndpoint::sendto()", errbuf, this);	
		}
	}
	NLDebug::dump((char *) buf, rc, "NLEndpoint sent:");
	return rc;
}


size_t NLEndpoint::sendto(NLPacket &pack, const NLAddress &to, int flags)
{
	int rc;

	rc = sendto((void *) pack.getData(), pack.getSize(), to, flags);
	return rc;

}



size_t NLEndpoint::recvfrom(void *buf, size_t size, NLAddress &from, int flags)
{
	long rc;
	struct sockaddr_in sa;
	int s = sizeof(sa);
	
	if(m_timeout >= 0 && isDataPending(m_timeout) == false)
		return 0;
		
	rc = ::recvfrom(m_socket, (char *) buf, size, flags, (struct sockaddr *) &sa, &s);
	if(rc < 0)
	{
		m_lasterr = errno;
		if(m_throwrecv == true)
		{
			char errbuf[256];
			sprintf(errbuf, "::recvfrom() failed: %s", strerror(errno));
			NL_THROW_ARG("NLEndpoint::recvfrom()", errbuf, this);	
		}
	}
	NLDebug::dump((char *) buf, rc, "NLEndpoint received:");
	from.set(sa);
	return rc;

}


size_t NLEndpoint::recv(NLPacket &pack, size_t size, int flags)
{
	otto_ptr<char> recbuf((char *) new char[size]);
	char errbuf[256];
	int rc;
	
	if(recbuf.get() == 0)
	{
		sprintf(errbuf, "new failed allocating %d bytes: %s", size, strerror(errno));
		NL_THROW_ARG("NLEndpoint::recv()", errbuf, this);	
	}
	
	rc = recv(recbuf.get(), size, flags);

	pack.insert((void *) recbuf.get(), rc);
	return rc;
}


size_t NLEndpoint::recvfrom(NLPacket &pack, size_t size, NLAddress &from, int flags)
{
	otto_ptr<char> recbuf((char *) new char[size]);
	char errbuf[256];
	int rc;
	
	if(recbuf.get() == 0)
	{
		sprintf(errbuf, "new failed allocating %d bytes: %s", size, strerror(errno));
		NL_THROW_ARG("NLEndpoint::recvfrom()", errbuf, this);	
	}
	
	rc = recvfrom(recbuf.get(), size, from, flags);
	
	pack.insert((void *) recbuf.get(), rc);
	return rc;
}


bool NLEndpoint::isDataPending(long timeout)
{
    fd_set fd;  
    struct timeval tv, *tv1 = 0; 
    char errbuf[256];

    if(timeout > 0)
    {
	
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	tv1 = &tv;
    }
    
    FD_ZERO(&fd);
    FD_SET(m_socket, &fd);
    
    if (::select(m_socket + 1, &fd, 0, 0, tv1) != -1)
    {	
	if (FD_ISSET(m_socket, &fd))
	    return true;
    } else
    {
	sprintf(errbuf, "::select failed: %s", strerror(errno));
	NL_THROW_ARG("NLEndpoint::isDataPending()", errbuf, this);	
    }
    
    return false;

}




