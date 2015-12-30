/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cassert>
#include <cstdarg>
#include <alloca.h>
#include "types.h"
#include "console.hh"
#include <string.h>
#include "memory.hh"
#include "arch-setup.hh"
#include "arch-cpu.hh"
#include "processor.hh"
#include "pagetable.hh"
#include "exceptions.hh"
#include "acpi.hh"
#include "smp.hh"
#include "apic.hh"
#include "logger.hh"

extern "C" {
    void premain();
}

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
{
    logger::info(logger::boot, "%s:%u:%d: %s\n", file, line, function, assertion);
    __builtin_trap();
}

interrupt_descriptor_table *gidt;
    extern "C" void interrupt(void *, int);

// note, we have no OS, hence, init ELF section doesn't run
// we need to run our own compiler
sched::arch_cpu arch_cpu;

static void boot_print(const char *fmt, ...)
{
    va_list va;
    va_start(va,fmt);
    logger::vinfo(logger::boot, fmt, va);
    va_end(va);
}

void test_interrupts();
void manual_set_pagetable();
void print_pagetable();

int main(int, char**);

void premain()
{
    // copy to stack so we don't free it now
    auto omb = *osv_multiboot_info;
    auto mb = omb.mb;
    char *e820_buffer = reinterpret_cast<char*>(alloca(mb.mmap_length));
    auto e820_size = mb.mmap_length;
    //char *mbe820 = reinterpret_cast<char*>(mb.mmap_addr);
    memcpy(e820_buffer, reinterpret_cast<void*>(mb.mmap_addr), e820_size);
    for_each_e820_entry(e820_buffer, e820_size, [] (e820ent ent) {
        logger::debug(logger::boot, "%x %x %d\n", ent.type, ent.addr, ent.size);
        memory::max_page_addr = reinterpret_cast<u8 *>(ent.addr);
        memory::max_page_addr += ent.size;
    });

    logger::init();
    logger::set(logger::boot, logger::level::DEBUG);
    logger::info(logger::boot, "miniOSV\n");

    interrupt_descriptor_table tbl;
    tbl.load_on_cpu();

    acpi::early_init();
    acpi::init();

    static ulong edata;
    asm ("movl $.edata, %0" : "=rm"(edata));

    new (&arch_cpu) sched::arch_cpu();
    arch_cpu.init_on_cpu();

    smp::init();
    smp::launch();

    main(0, NULL);

    acpi::poweroff();
}

void test_interrupts()
{
    interrupts::register_fn(50, [](void *) {
        logger::debug(logger::boot, "interrupt 50\n");
        return 0u;
    });
    asm volatile ("int $50");
    asm volatile ("int $51");
}

void manual_set_pagetable()
{
    //memory::alloc_page();
    mmu::cr3 cr3{processor::read_cr3()};
    logger::info(logger::boot, (char*)"cr3:  ");
    cr3.print(boot_print);
    logger::info(logger::boot, (char*)"\n");
    mmu::pml4e *pml4 = &cr3.PML4ptr()[511];
    mmu::init(pml4);
    //u8 unused = *reinterpret_cast<u8*>(0xfffffff100);
    // *reinterpret_cast<u8*>(0xfffffff100) = 1;
    pml4->PDPTptr(memory::alloc_page());
    mmu::pdpte *pdpt = pml4->PDPTptr()+4;
    mmu::init(pdpt->to_pd());
    pdpt->type(mmu::pdpt_type::PDPT_PD);
    pdpt->to_pd()->pd(memory::alloc_page());
    mmu::pde *pd = pdpt->to_pd()->pd();
    mmu::init(pd->to_pt());
    pd->type(mmu::pd_type::PD_PT);
    pd->to_pt()->pt(memory::alloc_page());
    mmu::pte *pt = pd->to_pt()->pt();
    mmu::init(pt);
    pt->page(memory::alloc_page());

    u8 *phys = reinterpret_cast<u8 *>(0x7ffa000);
    *phys = 0xFA;
    mmu::vaddr virt{cr3, pml4, pdpt, pd, pt, 0};
    logger::info(logger::boot, (char*)"%x\n", *phys);
    logger::info(logger::boot, (char*)"PML4() :%x\n", virt.PML4());
    logger::info(logger::boot, "got %x\n", *virt.ptr());
    *virt.ptr() = 10;
    logger::info(logger::boot, (char*)"XXX %0x%0x\n", virt.to_u64()>>32, (u32)virt.to_u64());
}

void print_pagetable()
{
    logger::info(logger::boot, "max_page_addr: %x\n", memory::max_page_addr-4096);
    auto pml4 = processor::read_cr3().PML4ptr();
    for (int i{0}; i<512;i++) {
        if (pml4[i].present == 0) continue;
        pml4[i].print(boot_print);
        logger::info(logger::boot, (char*)"\n");
        mmu::pdpte *pdpt = pml4[i].PDPTptr();
        for (int j{0}; j<512; j++) {
            if (pdpt[j].present() == 0) continue;
            /*pdpt[j].print(printf);
            printf((char*)"\n");*/
            if (pdpt[j].type() == mmu::pdpt_type::PDPT_1G) continue;
            mmu::pde *pd = pdpt[j].to_pd()->pd();
            for (int k{0}; k<512; k++) {
                if (!pd[k].present()) continue;
                pd[k].print(boot_print);
                logger::info(logger::boot, (char*)"\n");
                if (pd[k].type() == mmu::pd_type::PD_2M) continue;
                mmu::pte *pt = pd[k].to_pt()->pt();
                for (int l{0}; l<512; l++) {
                    if (!pt[l].present) continue;
                    pt[l].print(boot_print);
                    logger::info(logger::boot, (char*)"\n");
                }
            }
        }
    }
}

