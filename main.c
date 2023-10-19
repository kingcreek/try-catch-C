#include "exception_handling.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void print_nbr(int x)
{
	if (x < 0)
		throw(EX_RANGE);
	printf("%d\n", x);
}

int main()
{
	int nbr = -1;
	try
	{
		print_nbr(nbr); // The function may throw 1st time.
	}
	catch (EX_RANGE)
	{
		// This will attempt the operation repeatedly until the function stops generating exceptions 
		// (not advisable unless you are confident that this is a valid approach).

		printf("Error handling. Trying again...\n");
		nbr++;
		retry();
	}
	catch (EX_SIGINT)
	{
		printf("Interrupted, the try catch is no longer attempted");
	}
	finally
	{
		printf("It will always show\n");
	}

	try
	{
		int a = 10 / 0;
	}
	catch_all
	{
		printf("Floating point exception!!\n");
	}

	try
	{
		char *a = NULL;
		a[51] = 'a';
	}
	catch_all
	{
		printf("Segmentation fault exception!!\n");
		print_error();
	}

	printf("End of program, bye bye\n");

	return 0;
}