/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cassert>
#include <cstdarg>
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
void print_pagetable();

int main(int, char**);

void premain()
{
    // copy to stack so we don't free it now
    auto omb = *osv_multiboot_info;
    auto mb = omb.mb;
    char *e820_buffer = reinterpret_cast<char*>(__builtin_alloca(mb.mmap_length));
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

void print_pagetable()
{
    logger::info(logger::boot, "max_page_addr: %x\n", memory::max_page_addr-4096);
    auto pml4 = processor::read_cr3().PML4ptr();
    for (int i{0}; i<512;i++) {
        if (pml4[i].present == 0) continue;
        pml4[i].print(boot_print);
        logger::info(logger::boot, " %d\n", i);
        mmu::pdpte *pdpt = pml4[i].PDPTptr();
        for (int j{0}; j<512; j++) {
            if (pdpt[j].present() == 0) continue;
            pdpt[j].print(boot_print);
            logger::info(logger::boot, " %d\n", j);
            if (pdpt[j].type() == mmu::pdpt_type::PDPT_1G) continue;
            mmu::pde *pd = pdpt[j].to_pd()->pd();
            for (int k{0}; k<512; k++) {
                if (!pd[k].present()) continue;
                pd[k].print(boot_print);
                logger::info(logger::boot, " %d\n", k);
                if (pd[k].type() == mmu::pd_type::PD_2M) {
                    mmu::vaddr addr{0ul};
                    addr._2m.PML4 = i;
                    addr._2m.directoryPtr = j;
                    addr._2m.directory = k;
                    logger::info(logger::boot, "addr %lx\n", addr.to_u64());
                    continue;
                }
                mmu::pte *pt = pd[k].to_pt()->pt();
                for (int l{0}; l<512; l++) {
                    if (!pt[l].present) continue;
                    pt[l].print(boot_print);

                    mmu::vaddr addr{0ul};
                    addr._4k.PML4 = i;
                    addr._4k.directoryPtr = j;
                    addr._4k.directory    = k;
                    addr._4k.table        = l;
                    logger::info(logger::boot, " %d\naddr %lx (%d, %d, %d, %d)\n", l, addr.to_u64(), i, j, k, l);
                }
            }
        }
    }
}

