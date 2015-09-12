#include "myBaseTools.h"

void intToChar(int n,char *numstr)
{
	int i,j;
	i=0;
	for(i=0;n/10;i++)
	{
		numstr[i]=n%10+'0';
		n=n/10;
	}
	numstr[i]=n%10+'0';
	numstr[i+1]=0;
	strrev(numstr);
}