#include "smp.hh"
#include <cassert>
#include "acpi.hh"
#include "printf.h"
extern "C" {
    #include "acpi.h"
}

namespace smp {
StackVector<cpu, max_ncpu> cpus;

void parse_madt()
{
    new (&cpus) StackVector<cpu, max_ncpu>();
    char madt_sig[] = ACPI_SIG_MADT;
    ACPI_TABLE_HEADER* madt_header;
    auto st = AcpiGetTable(madt_sig, 0, &madt_header);
    assert(st == AE_OK);
    auto madt = reinterpret_cast<ACPI_TABLE_MADT*>(madt_header);
    void* subtable = madt + 1;
    void* madt_end = static_cast<void*>(madt) + madt->Header.Length;
    int nr_cpus = 0;
    while (subtable != madt_end) {
        auto s = static_cast<ACPI_SUBTABLE_HEADER*>(subtable);
        switch (s->Type) {
        case ACPI_MADT_TYPE_LOCAL_APIC: {
            auto lapic = reinterpret_cast<ACPI_MADT_LOCAL_APIC*>(s);
            if (!(lapic->LapicFlags & ACPI_MADT_ENABLED)) {
                break;
            }
            cpu c{nr_cpus++};
            c.apic_id = lapic->Id;
            c.acpi_id = lapic->ProcessorId;
            //c->arch.initstack.next = smp_stack_free;
            //smp_stack_free = &c->arch.initstack;
            cpus->push_back(c);
            break;
        }
        default:
            break;
        }
        subtable += s->Length;
    }
    printf("%d CPUs detected\n", nr_cpus);
}

}
