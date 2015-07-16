#ifndef _MEMCPY_DECODE_HH
#define _MEMCPY_DECODE_HH

#include "types.h"
#include <algorithm>

struct exception_frame {
    ulong r15;
    ulong r14;
    ulong r13;
    ulong r12;
    ulong r11;
    ulong r10;
    ulong r9;
    ulong r8;
    ulong rbp;
    ulong rdi;
    ulong rsi;
    ulong rdx;
    ulong rcx;
    ulong rbx;
    ulong rax;
    u16 error_code;
    ulong rip;
    ulong cs;
    ulong rflags;
    ulong rsp;
    ulong ss;

    void *get_pc(void) { return (void*)rip; }
    unsigned int get_error(void) { return error_code; }
};


typedef void (*fixup_function)(exception_frame *ef, size_t fixup);
class memcpy_decoder {
private:
    unsigned long _pc;
    unsigned long _size;
    fixup_function _fixup_fn;
public:
    memcpy_decoder(unsigned long pc, fixup_function fn);
    bool operator<(const memcpy_decoder b) const {
        return _pc < b._pc;
    }

    bool operator==(const unsigned long pc) const {
        return _pc == pc;
    }

    void memcpy_fixup(exception_frame *ef, size_t fixup) { _fixup_fn(ef, fixup); }
    static unsigned char *dest(exception_frame *ef);
    static unsigned char *src(exception_frame *ef);
    size_t size(exception_frame *ef);
} __attribute__((packed));

memcpy_decoder *memcpy_find_decoder(exception_frame *ef);
#endif
