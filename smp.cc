#include "smp.hh"
#include <cassert>
#include "acpi.hh"
#include "log.hh"
#include "processor.hh"
#include "msr.hh"
#include "apic.hh"
extern "C" {
    #include "acpi.h"
}

extern char smpboot[], smpboot_end[];

extern u32 smpboot_cr0, smpboot_cr4;
extern u64 smpboot_efer, smpboot_cr3;


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
    logger::info(logger::boot, "%d CPUs detected\n", nr_cpus);
}

void init()
{
    using namespace processor;
    parse_madt();
    smpboot_cr0 = read_cr0();
    smpboot_cr4 = read_cr4();
    smpboot_efer = rdmsr(msr::IA32_EFER);
    smpboot_cr3 = read_cr3();
    memcpy(reinterpret_cast<void *>(0), smpboot, smpboot_end - smpboot);
    processor::apic = processor::create_apic_driver();
}

cpu* smp_initial_find_current_cpu()
{
    for (uint32_t i=0; i<cpus->size(); i++) {
        auto c = &cpus[i];
        if (c->apic_id == processor::apic->id()) {
            return c;
        }
    }
    abort();
}

volatile unsigned smp_processors = 1;

void launch()
{
    //ioapic::init(); - we assume 1:1 phys:linear mapping
    //processor::kvm_pv_eoi_init();
    auto boot_cpu = smp_initial_find_current_cpu();
    for (unsigned i=0; i<cpus->size(); i++) {
        auto c = &cpus[i];
        //auto name = osv::sprintf("balancer%d", c->id);
        if (c == boot_cpu) {
            /*sched::thread::current()->_detached_state->_cpu = c;
            // c->init_on_cpu() already done in main().
            (new sched::thread([c] { c->load_balance(); },
                    sched::thread::attr().pin(c).name(name)))->start();
            c->init_idle_thread();
            c->idle_thread->start();*/
            continue;
        }
        /*sched::thread::attr attr;
        attr.stack(81920).pin(c).name(name);
        c->init_idle_thread();
        c->bringup_thread = new sched::thread([=] { ap_bringup(c); }, attr, true);*/
        processor::apic->init_ipi(c->apic_id, 0x4500); // INIT
        processor::apic->init_ipi(c->apic_id, 0x4600); // SIPI
        processor::apic->init_ipi(c->apic_id, 0x4600); // SIPI
    }

    while (smp_processors != cpus->size()) {
        //barrier();
    }
}

}

extern "C" {
void smp_main(void)
{ 
    processor::apic->init_on_ap();
    auto cpu = smp::smp_initial_find_current_cpu();
    logger::info(logger::boot, "Started %d\n", cpu->id);
    __sync_fetch_and_add(&smp::smp_processors, 1);
    for(;;) processor::sti_hlt();
}
}

