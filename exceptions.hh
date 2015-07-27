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

#endif
