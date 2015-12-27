#include <string.h>
#include <cstddef>
#include <new>
#include "log.hh"
extern "C" void __cxa_pure_virtual()
{
    log::info(log::exception, "bad virtual function\n");
    while (1);
}
void operator delete(void *) { }
extern "C" {
void *__dso_handle;
 
int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso){ return 0; }
void __cxa_finalize(void *f){}
}
//void* operator new (std::size_t size) throw (std::bad_alloc) { printf("new is called\n"); for (;;); __builtin_unreachable(); }
namespace std {
void __throw_bad_function_call() {
    log::info(log::exception, "bad function called\n");
    for (;;);
}
void __throw_length_error(char const*)
{
    log::info(log::exception, "%s\n", __func__);
    for (;;);
}
void __throw_bad_alloc()
{
    log::info(log::exception, "%s\n", __func__);
    for (;;);
}
}

void *operator new(size_t)
{
    log::info(log::acpi, "%s\n", __func__);
    __builtin_abort();
}
