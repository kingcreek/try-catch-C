/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception_handling2.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 10:44:55 by imurugar          #+#    #+#             */
/*   Updated: 2023/11/14 19:08:51 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HANDLING_H
#define EXCEPTION_HANDLING_H

#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define EX_BEGIN 0xA0
#define SIG_BEGIN 0xA6
#define SIG_MAX 0x20

enum myexception
{
	EX_OVERFLOW = 0xA0,
	EX_RANGE = 0xA1,
	EX_EOF = 0xA2,
	EX_NULLPTR = 0xA3,
	EX_MEMORY = 0xA4,
	EX_FILE = 0xA5,
	EX_SIGFPE = 0xA6,
	EX_SIGILL = 0xA7,
	EX_SIGSEGV = 0xA8,
	EX_SIGABRT = 0xA9,
	EX_SIGINT = 0xAA,
	EX_SIGTERM = 0xAB
};

const char *_exception_string[] = {
	"EX_OVERFLOW",
	"EX_RANGE",
	"EX_EOF",
	"EX_NULLPTR",
	"EX_MEMORY",
	"EX_FILE",
	"EX_SIGFPE",
	"EX_SIGILL",
	"EX_SIGSEGV",
	"EX_SIGABRT",
	"EX_SIGINT",
	"EX_SIGTERM"
};

typedef struct
{
	jmp_buf _catch_jmp_buf;
	int _catch_value;
	volatile bool _retry_attempt;
	volatile bool _init_signals;
	volatile bool _trextrigger;
	volatile const char *_trexfile;
	volatile const char *_trexfunc;
	volatile unsigned long _trexline;
	volatile int _trex;
	volatile int catched;
} ExceptionContext;

#define MAX_EXCEPTION_CONTEXTS 50
_Thread_local ExceptionContext _ex_ctx[MAX_EXCEPTION_CONTEXTS] = {0};
_Thread_local volatile int _ex_i = -1;
_Thread_local volatile bool _init_signals = false;

void _catch_sig_handle(int sig);

#define _trace_prepare(X)							\
	do												\
	{												\
		_ex_ctx[_ex_i]._trextrigger = true;			\
		_ex_ctx[_ex_i]._trexfile = __FILE__;		\
		_ex_ctx[_ex_i]._trexfunc = __func__;		\
		_ex_ctx[_ex_i]._trexline = __LINE__ + 0UL;	\
		_ex_ctx[_ex_i]._trex = X;					\
	} while (false)


#define _trace_throw(X)						\
	fprintf(stderr,							\
			"[%s] %s:%lu: Exception %s.\n",	\
			_ex_ctx[_ex_i]._trexfile,		\
			_ex_ctx[_ex_i]._trexfunc,		\
			_ex_ctx[_ex_i]._trexline,		\
			_exception_string[_ex_ctx[_ex_i]._trex - EX_BEGIN])

#define _trace_signal()												\
	fprintf(stderr,													\
			"Interrupted by signal: %s.\n",							\
			_exception_string[_ex_ctx[_ex_i]._trex - EX_BEGIN])

#define print_error()	 							\
	do												\
	{							 					\
		if (_ex_ctx[_ex_i]._trextrigger)		 	\
		{											\
			if (_ex_ctx[_ex_i]._trex >= SIG_BEGIN)	\
			{										\
				_trace_signal();  					\
			}										\
			else				  					\
			{										\
				_trace_throw();						\
			}										\
		}											\
		_ex_ctx[_ex_i]._trextrigger = false;  		\
	} while (false)
	
#define _bind_signals()					 		\
	do											\
	{											\
		for (uint8_t i = 1; i < SIG_MAX; i++)	\
			signal(i, _catch_sig_handle);		\
		_init_signals = true;					\
	} while (false)

#define try							 										\
	if (!_init_signals)				 							   			\
		_bind_signals();			   										\
	_ex_i++;							  									\
	_ex_ctx[_ex_i].catched = false;											\
	_ex_ctx[_ex_i]._catch_value = setjmp(_ex_ctx[_ex_i]._catch_jmp_buf); 	\
	if (_ex_ctx[_ex_i]._retry_attempt)										\
	{									  									\
		_ex_ctx[_ex_i]._catch_value = 0;				 					\
		_ex_ctx[_ex_i]._retry_attempt = false;								\
		_ex_ctx[_ex_i].catched = false;										\
	}									  									\
	if (!_ex_ctx[_ex_i]._catch_value && _ex_ctx[_ex_i].catched == false)

#define catch(X)							   										\
	else if (_ex_ctx[_ex_i]._catch_value == X && _ex_ctx[_ex_i].catched == true) 	\
		_ex_i--; 																	\

#define catch_all  										\
	else if (_ex_ctx[_ex_i].catched == true)			\
		_ex_i--; 										\

#define finally

#define throw(X)										\
	do								 	  				\
	{													\
		_ex_ctx[_ex_i].catched = true;   				\
		_trace_prepare(X);			   				 	\
		longjmp(_ex_ctx[_ex_i]._catch_jmp_buf, X);	 	\
	} while (false)

#define retry()						  					\
	do								  			 		\
	{						  							\
		if (_ex_ctx[_ex_i + 1]._catch_value != 0)		\
		{	  											\
			_ex_i++; 									\
			_ex_ctx[_ex_i]._retry_attempt = true;		\
			longjmp(_ex_ctx[_ex_i]._catch_jmp_buf, 1);	\
		}   											\
	} while (false)

void _catch_sig_handle(int sig)
{
	switch (sig)
	{
	case SIGFPE:
		throw(EX_SIGFPE);
	case SIGILL:
		throw(EX_SIGILL);
	case SIGSEGV:
		throw(EX_SIGSEGV);
	case SIGABRT:
		throw(EX_SIGABRT);
	case SIGINT:
		throw(EX_SIGINT);
	case SIGTERM:
		throw(EX_SIGTERM);
#ifdef SIGCONT
	case SIGCONT:
		return;
#endif
	default:
		signal(sig, SIG_DFL);
		return;
	}
}

#endif
