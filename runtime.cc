#include "printf.h"
extern "C" void __cxa_pure_virtual() { printf("bad virtual function\n"); while (1); }
void operator delete(void *) { }
