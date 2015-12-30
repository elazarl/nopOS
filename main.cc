#include "logger.hh"
#include "spinlock.h"
#include "pagetable.hh"
#include "processor.hh"
#include "console.hh"
#include "smp.hh"

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

void volatile_write(void *ptr)
{
    volatile u8 *p = reinterpret_cast<volatile u8 *>(ptr);
    *p = *p;
}

int main(int ac, char **av)
{
    mmu::vaddr addr{0xffff1};
    *addr.ptr() = 1;
    auto pml4 = processor::read_cr3().PML4ptr();
    pml4->print(printer{});
    auto pdpt = pml4[addr.PML4()].PDPTptr();
    pdpt->print(printer{});
    auto pd = pdpt[addr.directoryPtr()].to_pd()->pd();
    pd->print(printer{});
    void *p = pd;

    barrier();
    printer{}("Press any key to start test");
    isa_serial_console_readch();
    volatile_write(p);
    printer{}("work press any key to exit");
    isa_serial_console_readch();
    return 0;
}

extern "C" void cpu_main(smp::cpu *cpu)
{
}
