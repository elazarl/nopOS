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

void memcpy(void *_dst, void *_src, size_t sz)
{
    char *dst = reinterpret_cast<char *>(_dst);
    char *src = reinterpret_cast<char *>(_src);
    for (size_t i = 0; i < sz; i++) {
        dst[i] = src[i];
    }
}

void *_memset(void *p, int val, size_t sz)
{
    char *dst = reinterpret_cast<char *>(p);
    for (size_t i =0; i<sz; i++) {
        dst[i] = val;
    }
    return p;
}

u8 *max_page_addr;

u8 *pagealloc() {
    max_page_addr -= 4096;
    _memset(max_page_addr, 0, 4096);
    return max_page_addr;
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
        printf((char *)"%x %x %d\n", ent.type, ent.addr, ent.size);
        max_page_addr = reinterpret_cast<u8 *>(ent.addr);
        max_page_addr += ent.size;
    });
    mmu::cr3 cr3{processor::read_cr3()};
    printf((char*)"cr3:  ");cr3.print(printf);printf((char*)"\n");
    mmu::pml4e *pml4 = &cr3.PML4ptr()[511];
    mmu::init(pml4);
    pml4->PDPTptr(pagealloc());
    mmu::pdpte *pdpt = pml4->PDPTptr();
    mmu::init(pdpt->to_pd());
    pdpt->type(mmu::pdpt_type::PDPT_PD);
    pdpt->to_pd()->pd(pagealloc());
    mmu::pde *pd = pdpt->to_pd()->pd();
    mmu::init(pd->to_pt());
    pd->type(mmu::pd_type::PD_PT);
    pd->to_pt()->pt(pagealloc());
    mmu::pte *pt = pd->to_pt()->pt();
    mmu::init(pt);
    pt->page(pagealloc());

    u8 *phys = reinterpret_cast<u8 *>(0x7ffa000);
    *phys = 0xFA;
    mmu::vaddr virt{cr3, pml4, pdpt, pd, pt, 0};
    printf((char*)"%x\n", *phys);
    printf((char*)"PML4() :%x\n", virt.PML4());
    printf("got %x\n", *virt.ptr());
    *virt.ptr() = 10;
    printf((char*)"XXX %0x%0x\n", virt.to_u64()>>32, (u32)virt.to_u64());
    return;

    pml4 = cr3.PML4ptr();
    for (int i{0}; i<512;i++) {
        if (pml4[i].present == 0) continue;
        pml4[i].print(printf);
            printf((char*)"\n");
        mmu::pdpte *pdpt = pml4[i].PDPTptr();
        for (int j{0}; j<512; j++) {
            if (pdpt[j].present() == 0) continue;
            pdpt[j].print(printf);
            printf((char*)"\n");
            if (pdpt[j].type() == mmu::pdpt_type::PDPT_1G) continue;
            mmu::pde *pd = pdpt[j].to_pd()->pd();
            for (int k{0}; k<512; k++) {
                if (!pd[k].present()) continue;
                pd[k].print(printf);
                printf((char*)"\n");
                if (pd[k].type() == mmu::pd_type::PD_2M) continue;
                mmu::pte *pt = pd[k].to_pt()->pt();
                for (int l{0}; l<512; l++) {
                    if (!pt[l].present) continue;
                    pt[l].print(printf);
                    printf((char*)"\n");
                }
            }
        }
    }
    printf((char*)"%x\n", virt.to_u64());
    printf((char*)"%x\n", virt._4k.directory);

    processor::outw( 0xB004, 0x0 | 0x2000 );
    processor::cli_hlt();
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

