/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

#include <stdint.h>
#include <functional>
#include "types.h"

struct error_code {
    // External event (bit 0) — When set, indicates that the exception occurred during delivery of an
    //event external to the program, such as an interrupt or an earlier exception.
    unsigned ext : 1;
    // Descriptor location (bit 1) — When set, indicates that the index portion of the error code refers
    // to a gate descriptor in the IDT; when clear, indicates that the index refers to a descriptor in the GDT
    // or the current LDT.
    unsigned idt : 1;
    // GDT/LDT (bit 2) — Only used when the IDT flag is clear. When set, the TI flag indicates that the
    // index portion of the error code refers to a segment or gate descriptor in the LDT; when clear, it indi-
    // cates that the index refers to a descriptor in the current GDT.
    unsigned ti : 1;
    unsigned segment_selector_index : 13;
} __attribute__((packed));

struct page_fault_error_code {
    // This flag is 0 if there is no translation for the linear address because the P flag was 0 in one of the paging-
    // structure entries used to translate that address.
    unsigned p : 1;
    // If the access causing the page-fault exception was a write, this flag is 1; otherwise, it is 0. This flag
    // describes the access causing the page-fault exception, not the access rights specified by paging.
    unsigned wr : 1;
    // If a user-mode access caused the page-fault exception, this flag is 1; it is 0 if a supervisor-mode access did
    // so. This flag describes the access causing the page-fault exception, not the access rights specified by
    // paging.
    unsigned us : 1;
    // This flag is 1 if there is no translation for the linear address because a reserved bit was set in one of the
    // paging-structure entries used to translate that address.
    unsigned rsvd : 1;
    // This flag is 1 if the access causing the page-fault exception was an instruction fetch. This flag describes the
    // access causing the page-fault exception, not the access rights specified by paging.
    unsigned id : 1;
    // This flag is 1 if the access causing the page-fault exception was a data access to a user-mode address with
    // protection key disallowed by the value of the PKRU register.
    unsigned pk : 1;
} __attribute__((packed));

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
    union {
        error_code regular;
        page_fault_error_code page_fault;
    } _error_code;
    ulong rip;
    ulong cs;
    ulong rflags;
    ulong rsp;
    ulong ss;

    void *get_pc(void) { return (void*)rip; }
    error_code get_error(void) { return _error_code.regular; }
    page_fault_error_code get_page_fault_error(void) { return _error_code.page_fault; }
};

class gsi_edge_interrupt;
class exception_frame;

class interrupt_descriptor_table {
public:
    interrupt_descriptor_table();
    void load_on_cpu();
    void register_interrupt(gsi_edge_interrupt *interrupt);
    void unregister_interrupt(gsi_edge_interrupt *interrupt);
    void invoke_interrupt(unsigned vector);

    /* TODO: after merge of MSI and Xen callbacks as interrupt class,
     * exposing these as 'public' should not be necessary anymore.
     */
    unsigned register_interrupt_handler(std::function<bool ()> pre_eoi,
                                        std::function<void ()> eoi,
                                        std::function<void ()> post_eoi);

    /* register_handler is a simplified way to call register_interrupt_handler
     * with no pre_eoi, and apic eoi.
     */
    unsigned register_handler(std::function<void ()> post_eoi);
    void unregister_handler(unsigned vector);

private:
    enum {
        type_intr_gate = 14,
    };
    enum {
        s_special = 0,
    };
    struct idt_entry {
        u16 offset0;
        u16 selector;
        u8 ist : 3;
        u8 res0 : 5;
        u8 type : 4;
        u8 s : 1;
        u8 dpl : 2;
        u8 p : 1;
        u16 offset1;
        u32 offset2;
        u32 res1;
    } __attribute__((aligned(16)));
    void add_entry(unsigned vec, unsigned ist, void (*handler)());
    idt_entry _idt[256];

};

extern interrupt_descriptor_table idt;

extern "C" {
    void page_fault(exception_frame* ef);
}

bool fixup_fault(exception_frame*);

namespace interrupts {
typedef unsigned int (*fn)(void *);
bool is_registered(int vec);
void register_fn(int vec, fn f);
}

#endif
