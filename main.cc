#include "logger.hh"
#include "spinlock.h"
#include "pagetable.hh"
#include "processor.hh"
#include "console.hh"
#include "smp.hh"
#include "memory.hh"
#include "cruntime.h"
#include <string.h>

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

struct readline {
    explicit readline(const char *fmt, ...)
    {
        va_list va;
        va_start(va,fmt);
        logger::vinfo(logger::boot, fmt, va);
        va_end(va);
    }
    ~readline()
    {
        isa_serial_console_readch();
    }
};

void inline __attribute__((always_inline)) volatile_write(mmu::vaddr addr)
{
    volatile u8 *p = reinterpret_cast<volatile u8 *>(addr.ptr());
    *p = *p;
}

void inline __attribute__((always_inline)) volatile_read(mmu::vaddr addr)
{
    volatile u8 __attribute__((unused)) sink;
    volatile u8 *p = reinterpret_cast<volatile u8 *>(addr.ptr());
    printer{}("Reading from %lx", addr.to_u64());
    sink = *p;
}

void print_pagetable();
void manual_set_pagetable(mmu::vaddr addr, u64 phys, void *free_page[]);
void fill_pages(void *pages[], int n)
{
    for (int i=0; i<n; i++) {
        pages[i] = memory::alloc_page();
        *reinterpret_cast<u8 *>(pages[i]) = i;
    }
}
int main(int ac, char **av)
{
    readline("running clac");
    asm volatile ("clac");
    readline("done running clac");
    mmu::vaddr addr{0xfffffff100};
    auto pml4 = processor::read_cr3().PML4ptr();
    pml4->print(printer{});
    void *free_pages1[4];
    void *free_pages2[4];
    fill_pages(free_pages1, 4);
    memcpy(free_pages2, free_pages1, sizeof(free_pages1));
    fill_pages(free_pages2+2, 1);
    manual_set_pagetable(addr, 0x7ffa000, free_pages1);

    barrier();
    readline("Reading from %lx", addr.to_u64());
    volatile_read(addr);
    readline("Mapping %lx to physical address %lx with different pages", addr.to_u64());
    manual_set_pagetable(addr, 0x7ffa000, free_pages2);
    processor::invlpg(addr.ptr());
    readline("Reading from %lx which is mapped via different pages", addr.to_u64());
    volatile_read(addr);
    //volatile_write(mmu::vaddr{&(cr3.PML4ptr()[addr._4k.PML4].PDPTptr()->to_pd()[addr._4k.directoryPtr])});
    readline("work press any key to exit");
    return 0;
}

extern "C" void cpu_main(smp::cpu *cpu)
{
}

template<typename T>
void print_mapping(mmu::vaddr addr, T printer)
{
    mmu::cr3 cr3{processor::read_cr3()};
    mmu::pml4e *pml4 = &cr3.PML4ptr()[addr._4k.PML4];
    mmu::pdpte *pdpt = &pml4->PDPTptr()[addr._4k.directoryPtr];
    if (pdpt->type() == mmu::pdpt_type::PDPT_1G) {
        return;
    }
    mmu::pde *pd = &pdpt->to_pd()->pd()[addr._4k.directory];
    mmu::pte *pt = &pd->to_pt()->pt()[addr._4k.table];
}

void manual_set_pagetable(mmu::vaddr addr, u64 phys, void *free_page[])
{
    mmu::cr3 cr3{processor::read_cr3()};
    mmu::pml4e *pml4 = &cr3.PML4ptr()[addr._4k.PML4];
    mmu::init(pml4);
    pml4->PDPTptr(free_page[0]);
    mmu::pdpte *pdpt = &pml4->PDPTptr()[addr._4k.directoryPtr];
    mmu::init(pdpt->to_pd());
    pdpt->type(mmu::pdpt_type::PDPT_PD);
    pdpt->to_pd()->pd(free_page[1]);
    mmu::pde *pd = &pdpt->to_pd()->pd()[addr._4k.directory];
    mmu::init(pd->to_pt());
    pd->type(mmu::pd_type::PD_PT);
    pd->to_pt()->pt(free_page[2]);
    mmu::pte *pt = &pd->to_pt()->pt()[addr._4k.table];
    mmu::init(pt);
    pt->page(free_page[3]);

    u8 *pphys = reinterpret_cast<u8 *>(phys);
    *pphys = 0xFA;
}

