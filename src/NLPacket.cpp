//
// NLPacket.cpp
// Networking library packet class
//
// Copyright 1997 by Howard Berkey
//

#include <netdb.h>
#include <netinet/in.h>
#include "NLPacket.h"
#include "NLException.h"

const size_t NLPacket::kGrowBy = 128; 


void NLPacket::p_growToHold(size_t addsize)
{
	unsigned char *oldbuf;
	
	if((m_trueSize - m_packSize) > addsize)
		return;
	
	oldbuf = m_data;
	m_data = new unsigned char[m_trueSize + addsize + NLPacket::kGrowBy];

	if(m_data == 0)
		NL_THROW_ARG("NLPacket::p_growToHold", "Out of memory", this);
	
	memset(m_data, 0, m_trueSize + addsize + NLPacket::kGrowBy);

	if(oldbuf != 0)
	  {
	    memcpy(m_data, oldbuf, m_packSize);
	    delete[] oldbuf;
	  }

	m_trueSize += addsize + NLPacket::kGrowBy;
}


NLPacket::NLPacket(size_t size)
{
	m_packSize = 0;
	m_trueSize = 0;
	m_removePos = 0;
	m_data = 0;

	if(size != 0)
		p_growToHold(size);
}



NLPacket::~NLPacket()
{
	delete[] m_data;
}




void NLPacket::insert(char data)
{
	size_t size = sizeof(data);
		
	p_growToHold(size);
	*(m_data + m_packSize) = (unsigned char) data;
	m_packSize += size;
}



void NLPacket::insert(unsigned char data)
{
	size_t size = sizeof(data);
		
	p_growToHold(size);
	*(m_data + m_packSize) = (unsigned char) data;
	m_packSize += size;

}



void NLPacket::insert(short data)
{
	size_t size = sizeof(data);
	short d;
		
	p_growToHold(size);
	d = htons(data);
	memcpy(m_data +m_packSize, &d, size);
	m_packSize += size;
}



void NLPacket::insert(unsigned short data)
{
	size_t size = sizeof(data);
	unsigned short d;
		
	p_growToHold(size);
	d = htons(data);
	memcpy(m_data +m_packSize, &d, size);
	m_packSize += size;

}



void NLPacket::insert(int data)
{
	size_t size = sizeof(long);
	long d = (long) data;
		
	p_growToHold(size);
	d = htonl(data);
	memcpy(m_data +m_packSize, &d, size);
	m_packSize += size;

}



void NLPacket::insert(unsigned int data)
{
	size_t size = sizeof(unsigned long);
	unsigned long d = (unsigned long) data;
			
	p_growToHold(size);
	d = htonl(data);
	memcpy(m_data +m_packSize, &d, size);
	m_packSize += size;

}



void NLPacket::insert(long data)
{
	size_t size = sizeof(data);
	long d = data;
			
	p_growToHold(size);
	d = htonl(data);
	memcpy(m_data +m_packSize, &d, size);
	m_packSize += size;

}



void NLPacket::insert(unsigned long data)
{
	size_t size = sizeof(data);
	unsigned long d = data;		
	p_growToHold(size);
	d = htonl(data);
	memcpy(m_data + m_packSize, &d, size);
	m_packSize += size;

}



void NLPacket::insert(float data)
{
	size_t size = sizeof(data);
		
	p_growToHold(size);
	memcpy(m_data + m_packSize, &data, size);
	m_packSize += size;

}



void NLPacket::insert(double data)
{
	size_t size = sizeof(data);
		
	p_growToHold(size);
	memcpy(m_data + m_packSize, &data, size);
	m_packSize += size;

}



void NLPacket::insert(const char *str)
{
	if(str == 0)
		return;
	p_growToHold(strlen(str) + 1);
	strcpy((char *) (m_data + m_packSize), str);
	m_packSize += strlen(str) + 1;

}

void NLPacket::insert(const void *data, size_t size)
{
	if(data == 0 || size <= 0)
		return;
	p_growToHold(size);
	memcpy(m_data + m_packSize, data, size);
	m_packSize += size;

}



bool NLPacket::p_verifySizeRemaining(size_t size)
{
	return (getBytesRemaining() >= size);
}


void NLPacket::remove(char &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(char)", 
			"Attempted read past end of packet", this);
			
	data = *(m_data + m_removePos);
	m_removePos += sizeof(char);
}



void NLPacket::remove(unsigned char &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(uchar)", 
			"Attempted read past end of packet", this);

	data = *(m_data + m_removePos);
	m_removePos += sizeof(unsigned char);
}



void NLPacket::remove(short &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(short)", 
			"Attempted read past end of packet", this);

	memcpy(&data, (m_data + m_removePos), sizeof(data));
	data = ntohs(data);
	m_removePos += sizeof(data);
}



void NLPacket::remove(unsigned short &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(ushort)", 
			"Attempted read past end of packet", this);

	memcpy(&data, (m_data + m_removePos), sizeof(data));
	data = ntohs(data);
	m_removePos += sizeof(data);
}



void NLPacket::remove(int &data)
{
	if(p_verifySizeRemaining(sizeof(long)) == false)
		NL_THROW_ARG("NLPacket::remove(int)", 
			"Attempted read past end of packet", this);
	long ldata;
	memcpy(&ldata, (m_data + m_removePos), sizeof(ldata));
	data = (int) ntohl(ldata);
	m_removePos += sizeof(long);
}



void NLPacket::remove(unsigned int &data)
{

	if(p_verifySizeRemaining(sizeof(long)) == false)
		NL_THROW_ARG("NLPacket::remove(uint)", 
			"Attempted read past end of packet", this);

	unsigned long ldata;
	memcpy(&ldata, (m_data + m_removePos), sizeof(ldata));
	data = (unsigned int) ntohl(ldata);
	m_removePos += sizeof(unsigned long);

}



void NLPacket::remove(long &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(long)", 
			"Attempted read past end of packet", this);
	memcpy(&data, (m_data + m_removePos), sizeof(data));
	data = ntohl(data);
	m_removePos += sizeof(long);
}



void NLPacket::remove(unsigned long &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(ulong)", 
			"Attempted read past end of packet", this);
	memcpy(&data, (m_data + m_removePos), sizeof(data));
	data = ntohl(data);
	m_removePos += sizeof(unsigned long);
}



void NLPacket::remove(float &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(float)", 
			"Attempted read past end of packet", this);

	memcpy(&data, (m_data + m_removePos), sizeof(data));
	m_removePos += sizeof(data);
}



void NLPacket::remove(double &data)
{
	if(p_verifySizeRemaining(sizeof(data)) == false)
		NL_THROW_ARG("NLPacket::remove(double)", 
			"Attempted read past end of packet", this);


	memcpy(&data, (m_data + m_removePos), sizeof(data));	
	m_removePos += sizeof(data);
}



void NLPacket::remove(char *data, size_t maxsize)
{
	char *c;

	c = (char *)(m_data + m_removePos);
	
	if(p_verifySizeRemaining(strlen(c)) == false && 
				p_verifySizeRemaining(maxsize) == false)
		NL_THROW_ARG("NLPacket::remove(char *, size_t)", 
			"Attempted read past end of packet", this);

	strncpy(data, c, maxsize);
	data[maxsize - 1] = '\0';
	m_removePos += strlen(c) + 1; // always eat the whole string

}



void NLPacket::remove(void *data, size_t size)
{	
	if(p_verifySizeRemaining(size) == false)
		NL_THROW_ARG("NLPacket::remove(void *, size_t)", 
			"Attempted read past end of packet", this);

	char *c = (char *)(m_data + m_removePos);
	memcpy(data, c, size);
	m_removePos += size; 
}



