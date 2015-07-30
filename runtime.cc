#include <string.h>
#include <cstddef>
#include <new>
#include "printf.h"
extern "C" void __cxa_pure_virtual() { printf("bad virtual function\n"); while (1); }
void operator delete(void *) { }
extern "C" {
void *__dso_handle;
 
int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso){ return 0; }
void __cxa_finalize(void *f){}
}
//void* operator new (std::size_t size) throw (std::bad_alloc) { printf("new is called\n"); for (;;); __builtin_unreachable(); }
namespace std {
    void __throw_bad_function_call() { printf("bad function called\n"); for (;;); }
}
extern "C" {
}
