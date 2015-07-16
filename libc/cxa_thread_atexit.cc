/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

// When a C++11 "thread_local" variable is first used in a thread, the C++
// compiler allocates an instance for this thread, and then calls the
// __cxa_thread_atexit_impl() function, which we implement here, to register
// the destructor to be called when this thread exits.
//
// The C++ compiler relies on the C library to implement this function,
// because its implementation is intertwined with the implementation of
// threads in the C library - i.e., only the C library can known when a
// thread end.
//
// The use of __cxa_thread_atexit_impl() is fairly new to gcc, and
// may not be relevant to other C++ compilers. Older versions of gcc did
// not use this function at all, and instead made use of Posix Threads'
// "Thread Specific Data" (pthread_key_create() et al.) - a portable
// technique of running certain callbacks each time a thread ends.

#include <memory>
#include <assert.h>

typedef void (*destructor) (void *);

extern "C"
void __cxa_thread_atexit_impl(destructor dtor, void* obj, void* dso_symbol)
{
}

static void __attribute__((constructor)) register_call_destructor()
{
}
