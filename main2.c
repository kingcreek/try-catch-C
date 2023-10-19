#include "exception_handling2.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void print_nbr(int x)
{
	printf("%d\n", x);
	if (x < 0)
		throw(EX_RANGE);
}

int main()
{
	try
	{
		try
		{
			
			//retry();
			char *a = NULL;
			a[51] = 'a';
		}
		catch_all
		{
			printf("Segmentation fault exception 0!!\n");
		}

		 char *a = NULL;
		 a[51] = 'a';
	}
	catch_all
	{
		printf("Segmentation fault exception 1!!\n");
	}

	printf("End of program, bye bye\n");

	return 0;
}