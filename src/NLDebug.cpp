//
// NLDebug.cpp
//
// Copyright 1998 by Howard Berkey
//

#include "NLException.h"
#include <stdio.h>
#include <ctype.h>

//
// If debugging things at static init time, feel free to change
// this to true so that static initializers will be debuggable
//
bool NLDebug::m_on = false;


void NLDebug::dump(const char *data, size_t len, const char *func)
{
	int i = 0;
	char linebuf[17];

	if(m_on == false)
		return;
		
	printf("----------\n%s\n(dumping %d bytes)\n", func, len);

	memset(linebuf, 0, sizeof(linebuf));
	while(i<len)
	{
		printf("%02x ", (unsigned char )data[i]);
		if(isprint(data[i]))
			linebuf[i % 16] = data[i];
		else
			linebuf[i % 16] = '.';
			
		if((i + 1) % 16 == 0) {
			printf("  |  %s\n", linebuf);
			memset(linebuf, 0, sizeof(linebuf));
			}
		i++;
	}
	if((i + 1)%16 != 0)
	{
		while(i % 16 != 0)
		{
			printf("   ");
			i++;
		}
		printf("  |  %s", linebuf);
	}
	printf("\n----------\n");
	fflush(stdout);
}


    
void NLDebug::print(const char *msg)
{
    if(m_on == false)
		return;
    
    if(msg == 0)
	msg = "";
    fprintf(stderr, "debug: %s\n", msg);
}





