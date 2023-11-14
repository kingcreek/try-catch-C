/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception_handling.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 10:44:55 by imurugar          #+#    #+#             */
/*   Updated: 2023/11/14 19:08:51 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HANDLING_H
#define EXCEPTION_HANDLING_H 1

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

_Thread_local jmp_buf _catch_jmp_buf = {0};
_Thread_local int _catch_value = 0;
_Thread_local volatile bool _retry_attempt = false;
_Thread_local volatile bool _init_signals = false;
_Thread_local volatile bool _trextrigger = false;
_Thread_local volatile const char *_trexfile = "";
_Thread_local volatile const char *_trexfunc = "";
_Thread_local volatile unsigned long _trexline = 0;
_Thread_local volatile int _trex = 0;

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


void _catch_sig_handle(int sig);

#define _trace_prepare(X)           \
	do                              \
	{                               \
		_trextrigger = true;        \
		_trexfile = __FILE__;       \
		_trexfunc = __func__;       \
		_trexline = __LINE__ + 0UL; \
		_trex = X;                  \
	} while (false)


#define _trace_throw(X)                      \
	fprintf(stderr,                          \
			"[%s] %s:%lu: Exception %s.\n",  \
			_trexfile, _trexfunc, _trexline, \
			_exception_string[_trex - EX_BEGIN])

#define _trace_signal()                     \
	fprintf(stderr,                         \
			"Interrupted by signal: %s.\n", \
			_exception_string[_trex - EX_BEGIN])

#define print_error()       		\
	do                              \
	{                               \
		if (_trextrigger)           \
		{							\
			if (_trex >= SIG_BEGIN) \
			{						\
				_trace_signal();    \
			}						\
			else                    \
			{						\
				_trace_throw();     \
			}						\
		}							\
		_trextrigger = false;       \
	} while (false)
	
#define _bind_signals()                       \
	do                                        \
	{                                         \
		for (uint8_t i = 1; i < SIG_MAX; i++) \
			signal(i, _catch_sig_handle);     \
		_init_signals = true;                 \
	} while (false)

#define try                                \
	if (!_init_signals)                    \
		_bind_signals();                   \
	_catch_value = setjmp(_catch_jmp_buf); \
	if (_retry_attempt)                    \
	{                                      \
		_catch_value = 0;                  \
		_retry_attempt = false;            \
	}                                      \
	if (!_catch_value)
	
#define finally

#define catch(X) \
	else if (_catch_value == X)

#define catch_all \
	else
	
#define throw(X)                         \
	do                                   \
	{                                    \
		_trace_prepare(X);               \
		longjmp(_catch_jmp_buf, X);      \
	} while (false)

#define retry()                          \
	do                                   \
	{                                    \
		if (_catch_value != 0)           \
		{                                \
			_retry_attempt = true;       \
			longjmp(_catch_jmp_buf, 1);  \
		}                                \
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
