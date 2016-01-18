/* $Id$ */

/* 7.15 Variable arguments <stdarg.h> 

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#ifndef _PDCLIB_STDARG_H
#define _PDCLIB_STDARG_H _PDCLIB_STDARG_H
#include <_PDCLIB_aux.h>
#include <_PDCLIB_config.h>
_PDCLIB_BEGIN_EXTERN_C

typedef __builtin_va_list va_list;

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)


_PDCLIB_END_EXTERN_C
#endif

