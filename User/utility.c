#include "utility.h"

void Delayus(int duration)
{
	while(duration--) 
	{
		int i=0x02;				
		while(i--)
		__asm("nop");
	}
}
