#include "logger.hh"
#include "spinlock.h"
#include "pagetable.hh"
#include "processor.hh"
#include "console.hh"
#include "smp.hh"
#include "memory.hh"

struct printer {
    void operator()(const char *fmt, ...)
    {
        va_list va;
        va_start(va,fmt);
        logger::vinfo(logger::boot, fmt, va);
        va_end(va);
    }
    ~printer() { logger::info(logger::boot, "\n"); }
};

void volatile_write(mmu::vaddr addr)
{
    volatile u8 *p = reinterpret_cast<volatile u8 *>(addr.ptr());
    *p = *p;
}

void print_pagetable();
void manual_set_pagetable(mmu::vaddr);
int main(int ac, char **av)
{
    mmu::vaddr addr{0xfffffff100};
    auto pml4 = processor::read_cr3().PML4ptr();
    pml4->print(printer{});
    manual_set_pagetable(addr);

    isa_serial_console_readch();
    volatile_write(addr);
    mmu::cr3 cr3{processor::read_cr3()};
    volatile_write(mmu::vaddr{&(cr3.PML4ptr()[addr._4k.PML4].PDPTptr()->to_pd()[addr._4k.directoryPtr])});
    printer{}("work press any key to exit");
    isa_serial_console_readch();
    return 0;
}

extern "C" void cpu_main(smp::cpu *cpu)
{
}

void manual_set_pagetable(mmu::vaddr addr)
{
    mmu::cr3 cr3{processor::read_cr3()};
    logger::info(logger::boot, (char*)"cr3:  ");
    cr3.print(printer{});
    mmu::pml4e *pml4 = &cr3.PML4ptr()[addr._4k.PML4];
    mmu::init(pml4);
    //u8 unused = *reinterpret_cast<u8*>(0xfffffff100);
    // *reinterpret_cast<u8*>(0xfffffff100) = 1;
    pml4->PDPTptr(memory::alloc_page());
    mmu::pdpte *pdpt = &pml4->PDPTptr()[addr._4k.directoryPtr];
    mmu::init(pdpt->to_pd());
    pdpt->type(mmu::pdpt_type::PDPT_PD);
    pdpt->to_pd()->pd(memory::alloc_page());
    mmu::pde *pd = &pdpt->to_pd()->pd()[addr._4k.directory];
    mmu::init(pd->to_pt());
    pd->type(mmu::pd_type::PD_PT);
    pd->to_pt()->pt(memory::alloc_page());
    mmu::pte *pt = &pd->to_pt()->pt()[addr._4k.table];
    mmu::init(pt);
    pt->page(memory::alloc_page());

    u8 *phys = reinterpret_cast<u8 *>(0x7ffa000);
    *phys = 0xFA;
}

