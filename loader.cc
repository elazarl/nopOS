/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cctype>
#include <cstdio>
#include "types.h"
#include "console.hh"
#include "mmu.hh"
#include <string.h>
#include "arch-setup.hh"
#include "processor.hh"
#include "pagetable.hh"
extern "C" {
#include "printf.h"
}

extern "C" {
    void premain();
}

/*void arch_init_early_console()
{
    console::isa_serial_console::early_init();
}*/

void memcpy(void *_dst, void *_src, size_t sz) {
    char *dst = reinterpret_cast<char *>(_dst);
    char *src = reinterpret_cast<char *>(_src);
    for (size_t i = 0; i < sz; i++) {
        dst[i] = src[i];
    }
}

void premain()
{
    isa_serial_console_early_init();
    debug_early("miniOSV\n");
    debug_early("woo hoo!\n");
    init_printf(nullptr, [](void *, char c) {
        isa_serial_console_putchar(c);
    });

    static ulong edata;
    asm ("movl $.edata, %0" : "=rm"(edata));
    // copy to stack so we don't free it now
    auto omb = *osv_multiboot_info;
    auto mb = omb.mb;
    char *e820_buffer = reinterpret_cast<char*>(alloca(mb.mmap_length));
    auto e820_size = mb.mmap_length;
    //char *mbe820 = reinterpret_cast<char*>(mb.mmap_addr);
    memcpy(e820_buffer, reinterpret_cast<void*>(mb.mmap_addr), e820_size);
    for_each_e820_entry(e820_buffer, e820_size, [] (e820ent ent) {
        //printf((char *)"%x %x %d\n", ent.type, ent.addr, ent.size);
    });
    mmu::cr3 cr3{processor::read_cr3()};
    printf((char*)"cr3:  ");cr3.print(printf);printf((char*)"\n");
    mmu::pml4e *pml4 = cr3.PML4ptr();
    for (int i{0}; i<512;i++) {
        if (pml4[i].present == 0) continue;
        pml4[i].print(printf);
            printf((char*)"\n");
        mmu::pdpte *pdpt = pml4[i].PDPTptr();
        for (int j{0}; j<512; j++) {
            if (pdpt[j].present() == 0) continue;
            pdpt[j].print(printf);
            printf((char*)"\n");
            if (pdpt[j].type() == mmu::pdpt_type::PDPT_PD) {
                mmu::pde *p = pdpt[j].to_pd().pd();
                for (int k{0}; k<512; k++) {
                    if (!p[k].present()) continue;
                    p[k].print(printf);
                    printf((char*)"\n");
                }
            }
        }
    }

    for (;;) {
        u8 b = isa_serial_console_readch();
        isa_serial_console_putchar(b);
        if (b == '\r') isa_serial_console_putchar('\n');
    }
    //arch_init_early_console();
#define OSV_VERSION "0"
    /* besides reporting the OSV version, this string has the function
       to check if the early console really works early enough,
       without depending on prior initialization. */
    //debug_early("OSv " OSV_VERSION "\n");


    /*auto inittab = elf::get_init(elf_header);
    setup_tls(inittab);
    boot_time.event("TLS initialization");
    for (auto init = inittab.start; init < inittab.start + inittab.count; ++init) {
        (*init)();
    }
    boot_time.event(".init functions");*/
}

int main(int ac, char **av)
{
    return 0;
}

extern "C" { void smp_main(void) {} }

