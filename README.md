# C Exception Handling

This repository contains a basic implementation of exception handling in C, inspired by the `try` and `catch` functionality commonly used in other programming languages.

## Description

Exception handling is a technique used to deal with errors and exceptional situations in a program. While C does not have native support for exceptions, this project provides a simple implementation of `try` and `catch` to help you handle errors more gracefully.

## Usage

Include `exception_handling.h` in your project.

You can use these `try`, `catch`, `finally` and `retry()` `print_error()` functions in your C programs to manage exceptions. Here's an example of how to use them

## TODO

Working in `exception_handling2.h` to able anidated `try`/`catch`. It is not completely necessary and would even seem like bad practice to me, but it does not hurt to allow this functionality.


Example `main.c`:

```c
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
```

